#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
// #include <tmmintrin.h>
#include <immintrin.h> ///AVX
#include "bmp.h"
#include "measure.h"
// #include <x86intrin.h>
#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include "Windows.h"
#include "malloc.h"
#define posix_memalign(address, alignment, size) *(address) = _aligned_malloc((size), (alignment))
#define sleep(s) Sleep(1000 * s)
#else
#include <cstring>
#include "unistd.h"
#define memcpy_s(d, n, s, c) memcpy(d, s, c)
#endif

/* just used for time measurements */
#define REP 10
#define MIN(X, Y) (((X) < (Y)) ? X : Y)

union vec_or_val
{
  __m256i value;
  unsigned char vector[32];
};

void kirsch_operator_avx(unsigned char *data_out, unsigned char *data_in, unsigned height, unsigned width)
{
  unsigned int row, col, lay;

  unsigned int size = height * width;
  unsigned num_of_ints_per_row = (width + 31) / 32 * 32;

  unsigned short *color_rows[3];

  /*
  For our sanity's sake, we divide the 3 colours into
  3 separate arrays. Each starting at an address that is 64-byte aligned.
  */

  // Allocate memory based on 64 bytes multiple
  // color_rows[0] is red, color_rows[1] is green, color_rows[2] is blue
  // Copy data into the allocated memory
  unsigned char *my_data_temp_in = data_in;
  for (lay = 0; lay < 3; lay++)
  {
    // Get pointer to each row
    color_rows[lay] = static_cast<unsigned short *>(std::aligned_alloc(64, sizeof(unsigned short) * num_of_ints_per_row * height));

    // Copy data
    for (row = 0; row < height; row++)
      for (col = 0; col < width; col++)
        color_rows[lay][row * num_of_ints_per_row + col] = my_data_temp_in[row * width + col];

    // Offset to next color group
    my_data_temp_in += size;
  }

  // Layer count, each time for a color row
  for (lay = 0; lay < 3; lay++)
  {
    // Now this loop is for the y rows.
    unsigned short *my_data_in_rmp = color_rows[lay];
    for (row = 1; row < height - 1; ++row)
    {
      __m256i*  row_ptr   [3];
      __m256i   rows      [3];
      __m256i   nextRows  [3];

      __m256i   rows5x    [3];
      __m256i   rows3x    [3];
      __m256i   nextRows5x[3];
      __m256i   nextRows3x[3];

      for (int i = 0, offset = -1; i < 3; ++i, ++offset)
      {
        row_ptr [i] = reinterpret_cast<__m256i*>(&my_data_in_rmp[(row + offset) * num_of_ints_per_row]);
        rows    [i] = _mm256_load_si256(row_ptr[i]++);
        nextRows[i] = _mm256_load_si256(row_ptr[i]);

        rows5x  [i] = _mm256_add_epi16(rows[i], _mm256_slli_epi16(rows[i], 2));
        nextRows5x  [i] = _mm256_add_epi16(nextRows[i], _mm256_slli_epi16(nextRows[i], 2));

        rows3x      [i] = _mm256_sub_epi16(rows[i], rows[i]);
        nextRows3x  [i] = _mm256_sub_epi16(nextRows[i], nextRows[i]);

        for (int j = 0; j < 3; ++j)
        {
          // -1, -2, -3
          rows3x[i]     = _mm256_sub_epi16(rows3x[i], rows[i]);
          nextRows3x[i] = _mm256_sub_epi16(nextRows3x[i], nextRows[i]);
        }

      }

      // j is pixel counter, adds 32 per loop, so it travels per pixel for the whole row
      // Again, it starts from j = 1 beause 0th pixel is ignored
      for (col = 1; col < width - 33; col += 32)
      {
        //Loop twice since promoted char to short. Want to calculate 32 pixels at 'once' before storing
        __m256i totalSum[2];
        for (int k = 0; k < 2; k++)
        {
          // Compute shifted rows
          __m256i rowsX3Shift0[3] = { rows3x[0], rows3x[1], rows3x[2] };
          __m256i rowsX3Shift1[3] = { _mm256_setzero_si256() };
          __m256i rowsX3Shift2[3] = { _mm256_setzero_si256() };

          __m256i rowsX5Shift0[3] = { rows5x[0], rows5x[1], rows5x[2] };
          __m256i rowsX5Shift1[3] = { _mm256_setzero_si256() };
          __m256i rowsX5Shift2[3] = { _mm256_setzero_si256() };

          // Variables to be reused
          __m256i uppMaskSum  = _mm256_setzero_si256();
          __m256i midMaskSum  = _mm256_setzero_si256();
          __m256i lowMaskSum  = _mm256_setzero_si256();

          // Compute shifts for X3 & X5 
          // We compute for the middle row, but we don't use it. It's just done for consistency's sake.
          for (int i = 0; i < 3; ++i)
          {
            // Do X3 first
            __m128i nextLow     = _mm256_extracti128_si256(nextRows3x[i], 0);
            __m128i currentLow  = _mm256_extracti128_si256(rows3x[i], 0);
            __m128i currentHigh = _mm256_extracti128_si256(rows3x[i], 1);

            // Shift 1
            __m128i shiftHigh   = _mm_or_si128(_mm_slli_si128(nextLow, 14), _mm_srli_si128(currentHigh, 2));
            __m128i shiftLow    = _mm_or_si128(_mm_slli_si128(currentHigh, 14), _mm_srli_si128(currentLow, 2));
            rowsX3Shift1[i]     = _mm256_set_m128i(shiftHigh, shiftLow);

            // Shift 2
            shiftHigh       = _mm_or_si128(_mm_slli_si128(nextLow, 12), _mm_srli_si128(currentHigh, 4));
            shiftLow        = _mm_or_si128(_mm_slli_si128(currentHigh, 12), _mm_srli_si128(currentLow, 4));
            rowsX3Shift2[i] = _mm256_set_m128i(shiftHigh, shiftLow);

            // Do the same for X5
            nextLow     = _mm256_extracti128_si256(nextRows5x[i], 0);
            currentLow  = _mm256_extracti128_si256(rows5x[i], 0);
            currentHigh = _mm256_extracti128_si256(rows5x[i], 1);

            // Shift 1
            shiftHigh       = _mm_or_si128(_mm_slli_si128(nextLow, 14), _mm_srli_si128(currentHigh, 2));
            shiftLow        = _mm_or_si128(_mm_slli_si128(currentHigh, 14), _mm_srli_si128(currentLow, 2));
            rowsX5Shift1[i] = _mm256_set_m128i(shiftHigh, shiftLow);

            // Shift 2
            shiftHigh       = _mm_or_si128(_mm_slli_si128(nextLow, 12), _mm_srli_si128(currentHigh, 4));
            shiftLow        = _mm_or_si128(_mm_slli_si128(currentHigh, 12), _mm_srli_si128(currentLow, 4));
            rowsX5Shift2[i] = _mm256_set_m128i(shiftHigh, shiftLow);
          }

          // Compute the masks
          __m256i maskSums[8] = { _mm256_setzero_si256() };

          /*
            Mask 1:
            +5 +5 +5
            -3  0 -3
            -3 -3 -3
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX5Shift0[0], _mm256_add_epi16(rowsX5Shift1[0], rowsX5Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX3Shift0[1], rowsX3Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX3Shift0[2], _mm256_add_epi16(rowsX3Shift1[2], rowsX3Shift2[2]));
          maskSums[0] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          /*
            Mask 2:
            +5 +5 -3
            +5  0 -3
            -3 -3 -3
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX5Shift0[0], _mm256_add_epi16(rowsX5Shift1[0], rowsX3Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX5Shift0[1], rowsX3Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX3Shift0[2], _mm256_add_epi16(rowsX3Shift1[2], rowsX3Shift2[2]));
          maskSums[1] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          /*
            Mask 3:
            +5 -3 -3
            +5  0 -3
            +5 -3 -3
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX5Shift0[0], _mm256_add_epi16(rowsX3Shift1[0], rowsX3Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX5Shift0[1], rowsX3Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX5Shift0[2], _mm256_add_epi16(rowsX3Shift1[2], rowsX3Shift2[2]));
          maskSums[2] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          /*
            Mask 4:
            -3 -3 -3
            +5  0 -3
            +5 +5 -3
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX3Shift0[0], _mm256_add_epi16(rowsX3Shift1[0], rowsX3Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX5Shift0[1], rowsX3Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX5Shift0[2], _mm256_add_epi16(rowsX5Shift1[2], rowsX3Shift2[2]));
          maskSums[3] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          /*
            Mask 5:
            -3 -3 -3
            -3  0 -3
            +5 +5 +5
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX3Shift0[0], _mm256_add_epi16(rowsX3Shift1[0], rowsX3Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX3Shift0[1], rowsX3Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX5Shift0[2], _mm256_add_epi16(rowsX5Shift1[2], rowsX5Shift2[2]));
          maskSums[4] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          /*
            Mask 6:
            -3 -3 -3
            -3  0 +5
            -3 +5 +5
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX3Shift0[0], _mm256_add_epi16(rowsX3Shift1[0], rowsX3Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX3Shift0[1], rowsX5Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX3Shift0[2], _mm256_add_epi16(rowsX5Shift1[2], rowsX5Shift2[2]));
          maskSums[5] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          /*
            Mask 7:
            -3 -3 +5
            -3  0 +5
            -3 -3 +5
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX3Shift0[0], _mm256_add_epi16(rowsX3Shift1[0], rowsX5Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX3Shift0[1], rowsX5Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX3Shift0[2], _mm256_add_epi16(rowsX3Shift1[2], rowsX5Shift2[2]));
          maskSums[6] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          /*
            Mask 8:
            -3 +5 +5
            -3  0 +5
            -3 -3 -3
          */
          
          uppMaskSum  = _mm256_add_epi16(rowsX3Shift0[0], _mm256_add_epi16(rowsX5Shift1[0], rowsX5Shift2[0]));
          midMaskSum  = _mm256_add_epi16(rowsX3Shift0[1], rowsX5Shift2[1]);
          lowMaskSum  = _mm256_add_epi16(rowsX3Shift0[2], _mm256_add_epi16(rowsX3Shift1[2], rowsX3Shift2[2]));
          maskSums[7] = _mm256_add_epi16(uppMaskSum, _mm256_add_epi16(midMaskSum, lowMaskSum));

          __m256i tempMaxSum = _mm256_setzero_si256();
          for (int i = 0; i < 8; ++i)
            tempMaxSum = _mm256_max_epi16(tempMaxSum, maskSums[i]);

          // Assign it back in here
          totalSum[k] = _mm256_srai_epi16(tempMaxSum, 3);

          for (int i = 0; i < 3; ++i)
          {
            rows  [i] = nextRows  [i];
            rows5x[i] = nextRows5x[i];
            rows3x[i] = nextRows3x[i];

            row_ptr[i]++;
            nextRows[i] = _mm256_load_si256(row_ptr[i]);
            nextRows5x[i] = _mm256_add_epi16(nextRows[i], _mm256_slli_epi16(nextRows[i], 2));

            nextRows3x  [i] = _mm256_sub_epi16(nextRows[i], nextRows[i]);
            for (int j = 0; j < 3; ++j)
              nextRows3x[i] = _mm256_sub_epi16(nextRows3x[i], nextRows[i]);
          }
        }

        // This is the packing portion, we compress 2 _m256i into 1 and store it into memory
        // but need to take care of packed 16-bit integers. Refer to instrincis manual.
        __m128i total_sum1_low = _mm256_extracti128_si256(totalSum[0], 0x0); // low
        __m128i total_sum1_hi = _mm256_extracti128_si256(totalSum[0], 0x1);  // high
        __m128i total_sum2_low = _mm256_extracti128_si256(totalSum[1], 0x0); // low
        __m128i total_sum2_hi = _mm256_extracti128_si256(totalSum[1], 0x1);  // high
        totalSum[0] = _mm256_set_m128i(total_sum2_low, total_sum1_low);
        totalSum[1] = _mm256_set_m128i(total_sum2_hi, total_sum1_hi);

        __m256i total_sum = _mm256_packus_epi16(totalSum[0], totalSum[1]);

        vec_or_val real_sum;
        _mm256_store_si256(&real_sum.value, total_sum);

        memcpy_s(&data_out[lay * size + row * width + col], 32, real_sum.vector, 32);
      }

      // This is the clean up part!
      // The image size might not be multiple of 16, so we need to deal with the remaining pixels manually.
      // Notice that this code here is pretty much copy pasted from the basis algorithm

      /* Kirsch matrices for convolution */
      int kirsch[8][3][3] = 
      {
        {
          {  5,  5,  5 },
          { -3,  0, -3 },  /*rotation 1 */
          { -3, -3, -3 }
        },
        {
          { 5,  5, -3 },
          { 5,  0, -3 },  /*rotation 2 */
          {-3, -3, -3 }
        },
        { 
          { 5, -3, -3 },
          { 5,  0, -3 },  /*rotation 3 */
          { 5, -3, -3 }
        },
        {
          { -3, -3, -3 },
          {  5,  0, -3 },  /*rotation 4 */
          {  5,  5, -3 }
        },
        {
          { -3, -3, -3 },
          { -3,  0, -3 },  /*rotation 5 */
          {  5,  5,  5 }
        },
        {
          { -3, -3, -3 },
          { -3,  0,  5 },  /*rotation 6 */
          { -3,  5,  5 }
        },
        {
          { -3, -3,  5 },
          { -3,  0,  5 },  /*rotation 7 */
          { -3, -3,  5 } 
        },
        {
          { -3,  5,  5 },
          { -3,  0,  5 },  /*rotation 8 */
          { -3, -3, -3 }
        }
      };

      for (; col < width - 1; col++)
      {
        int maxSum = 0;
        for (unsigned mask = 0; mask < 8; ++mask) 
        {
          int sum = 0;

          /* Convolution part */
          for (int maskRow = -1; maskRow < 2; ++maskRow)
            for (int maskCol = -1; maskCol < 2; ++maskCol)
            {
              int maskRowIndex = maskRow + 1;
              int maskValIndex = maskCol + 1;

              int incPixelIndex = lay * size + (row + maskRow) * width + (col + maskCol);
              int incPixel = static_cast<int>(data_in[incPixelIndex]);

              sum += kirsch[mask][maskRow][maskCol] * incPixel;
            }

            if (sum > maxSum)
              maxSum = sum;
        }

        maxSum = (maxSum / 8) > 255 ? 255: (maxSum / 8);

        const int OUTPUT_INDEX = lay * size + row * width + col;
        data_out[OUTPUT_INDEX] = maxSum;
      }
    }
  }
  for (int i = 0; i < 3; i++)
    std::free(color_rows[i]);
}



#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
//#include <tmmintrin.h>
#include <immintrin.h> ///AVX
#include "bmp.h"
#include "measure.h"
//#include <x86intrin.h>
#include <iostream>
#include <iomanip>
#ifdef _WIN32
  #include "Windows.h"
  #include "malloc.h"
  #define posix_memalign(address, alignment, size) *(address)=_aligned_malloc((size), (alignment))
  #define sleep(s) Sleep(1000*s)
#else
#include <cstring>
#include "unistd.h"
  #define memcpy_s(d, n, s, c) memcpy(d, s, c)
#endif

/* just used for time measurements */
#define REP             10
#define NUM_LAYERS      3
#define NUM_ROWS        3
#define NUM_MASKS       8
#define NUM_SHIFTS      3 // 0, 1, 2
#define MIN             (X, Y) (((X)<(Y))?X:Y)

//=======================================================================================
// Type Definitions
//=======================================================================================

union _256toUChar
{
  __m256i         value = _mm256_setzero_si256();
  unsigned char   vector[32];
};

//=======================================================================================
// Local Variables
//=======================================================================================

/* Kirsch matrices for convolution */
const int KIRSCH_MASKS[NUM_MASKS][NUM_ROWS][NUM_ROWS] = 
{
  {
    { 5,  5,  5 },
    {-3,  0, -3 },  /*rotation 1 */
    {-3, -3, -3 }
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
    {-3, -3, -3 },
    { 5,  0, -3 },  /*rotation 4 */
    { 5,  5, -3 }
    },
  {
    {-3, -3, -3 },
    {-3,  0, -3 },  /*rotation 5 */
    { 5,  5,  5 }
    },
  {
    {-3, -3, -3 },
    {-3,  0,  5 },  /*rotation 6 */
    {-3,  5,  5 }
    },
  {
    {-3, -3,  5 },
    {-3,  0,  5 },  /*rotation 7 */
    {-3, -3,  5 }
    },
  {
    {-3,  5,  5 },
    {-3,  0,  5 },  /*rotation 8 */
    {-3, -3, -3 }
  }
};

//=======================================================================================
// Function Definitions
//=======================================================================================

void kirsch_operator_avx
(
  unsigned char*  data_out
, unsigned char*  data_in
, unsigned        height
, unsigned        width
)
{
  // Variables for iterations
  unsigned int row    = 0;
  unsigned int col    = 0;
  unsigned int layer  = 0;

  // Determine the number of ints per row. We need it to be a multiple of 32 due to AVX 256-bit registers.
  const unsigned int SIZE             = height * width;
  const unsigned int NUM_INTS_PER_ROW = (width + 31) / 32 * 32;

  // Allocate colours into 3 short arrays and copy the data into memory
  unsigned short* colourRows[NUM_LAYERS] = { nullptr };

  unsigned char* tempDataIn = data_in;
  for (layer = 0; layer < NUM_LAYERS; ++layer)
  {
    colourRows[layer] = static_cast<unsigned short*>(std::aligned_alloc(64, sizeof(unsigned short) * NUM_INTS_PER_ROW * height));

    for (row = 0; row < height; ++row)
      for (col = 0; col < width; ++col)
        colourRows[layer][row * NUM_INTS_PER_ROW + col] = tempDataIn[row * width + col];
    
    // Offset to next colour group
    tempDataIn += SIZE;
  }

  // We perform the mask for each colour (layer)
  for (layer = 0; layer < NUM_LAYERS; ++layer)
  {
    unsigned short* data = colourRows[layer];

    // We apply the masks per row
    // We skip the first and last row since the masks cannot be applied there.
    for (row = 1; row < height - 1; ++row)
    {
      /*
        We need to maintain pointers to the upper, middle and lower rows
        These pointers are needed because we cannot compute the convolution 
        of the last element in the row without the next row iteration.

        So, we need prepare to store the next iteration as well as the current.
      */

    __m256i* rowPointers[NUM_ROWS] = { nullptr };
    __m256i  rows       [NUM_ROWS] = { _mm256_setzero_si256() };
    __m256i  rowsX3     [NUM_ROWS] = { _mm256_setzero_si256() };
    __m256i  rowsX5     [NUM_ROWS] = { _mm256_setzero_si256() };
    __m256i  nextRows   [NUM_ROWS] = { _mm256_setzero_si256() };
    __m256i  nextRowsX3 [NUM_ROWS] = { _mm256_setzero_si256() };
    __m256i  nextRowsX5 [NUM_ROWS] = { _mm256_setzero_si256() };

     for (int i = 0, offset = -1; i < NUM_ROWS; ++i, ++offset)
     {
      rowPointers[i] = reinterpret_cast<__m256i*>(&data[(row + offset) * NUM_INTS_PER_ROW]);

      // Load the current & next itertion of 16 pixels
      // We need to store the next iteraton to compute the convolution for the last value in the current iteration
      rows[i]     = _mm256_load_si256(rowPointers[i]++);
      nextRows[i] = _mm256_load_si256(rowPointers[i]);

      // Compute X3 & X5
      // We compute rowX3 as negative for ease of use when computing kirsch convolution sums

      // Compute X3: We subtract 4 times
      rowsX3[i]     = _mm256_sub_epi16(rows[i], rows[i]);           // 0
      nextRowsX3[i] = _mm256_sub_epi16(nextRows[i], nextRows[i]);

      for (int j = 0; j < 3; ++j)
      {
        // -1, -2, -3
        rowsX3[i]     = _mm256_sub_epi16(rowsX3[i], rows[i]);
        nextRowsX3[i] = _mm256_sub_epi16(nextRowsX3[i], nextRows[i]);
      }
        
      // Compute X5: Bitshift Left twice then add once
      rowsX5[i]     = _mm256_add_epi16(_mm256_slli_epi16(rows[i], 2), rows[i]);
      nextRowsX5[i] = _mm256_add_epi16(_mm256_slli_epi16(nextRows[i], 2), nextRows[i]);
     }

      for (col = 1; col < width - 33; col += 32)
      {
        /*
          We work on 3 values at a time in this format

          shift2  4 3 2  * maskVal
          shift1  3 2 1  * maskVal
          shift0  2 1 0  * maskVal
                  | | |   (sum)
                  3 2 1

          This is done for each row.

          Once we apply the masks, we divide the entire value by 8, then compare it to the max sum.
        */

        // We use 2 max sums since we can pack them later by removing the short portion.
        __m256i maxSum[2] = { _mm256_setzero_si256() };

        // Iterate twice for each max sum
        for (int maxSumCounter = 0; maxSumCounter < 2; ++maxSumCounter)
        {
          // Compute shifted rows
          __m256i rowsX3Shift0[NUM_ROWS] = { rowsX3[0], rowsX3[1], rowsX3[2] };
          __m256i rowsX3Shift1[NUM_ROWS] = { _mm256_setzero_si256() };
          __m256i rowsX3Shift2[NUM_ROWS] = { _mm256_setzero_si256() };

          __m256i rowsX5Shift0[NUM_ROWS] = { rowsX5[0], rowsX5[1], rowsX5[2] };
          __m256i rowsX5Shift1[NUM_ROWS] = { _mm256_setzero_si256() };
          __m256i rowsX5Shift2[NUM_ROWS] = { _mm256_setzero_si256() };

          // Variables to be reused
          __m256i uppMaskSum  = _mm256_setzero_si256();
          __m256i midMaskSum  = _mm256_setzero_si256();
          __m256i lowMaskSum  = _mm256_setzero_si256();

          // Compute shifts for X3 & X5 
          // We compute for the middle row, but we don't use it. It's just done for consistency's sake.
          for (int i = 0; i < NUM_ROWS; ++i)
          {
            // Do X3 first
            __m128i nextLow     = _mm256_extracti128_si256(nextRowsX3[i], 0);
            __m128i currentLow  = _mm256_extracti128_si256(rowsX3[i], 0);
            __m128i currentHigh = _mm256_extracti128_si256(rowsX3[i], 1);

            // Shift 1
            __m128i shiftHigh   = _mm_or_si128(_mm_slli_si128(nextLow, 14), _mm_srli_si128(currentHigh, 2));
            __m128i shiftLow    = _mm_or_si128(_mm_slli_si128(currentHigh, 14), _mm_srli_si128(currentLow, 2));
            rowsX3Shift1[i]     = _mm256_set_m128i(shiftHigh, shiftLow);

            // Shift 2
            shiftHigh       = _mm_or_si128(_mm_slli_si128(nextLow, 12), _mm_srli_si128(currentHigh, 4));
            shiftLow        = _mm_or_si128(_mm_slli_si128(currentHigh, 12), _mm_srli_si128(currentLow, 4));
            rowsX3Shift2[i] = _mm256_set_m128i(shiftHigh, shiftLow);

            // Do the same for X5
            nextLow     = _mm256_extracti128_si256(nextRowsX5[i], 0);
            currentLow  = _mm256_extracti128_si256(rowsX5[i], 0);
            currentHigh = _mm256_extracti128_si256(rowsX5[i], 1);

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
          __m256i maskSums[NUM_MASKS] = { _mm256_setzero_si256() };

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
          for (int i = 0; i < NUM_MASKS; ++i)
            tempMaxSum = _mm256_max_epi16(tempMaxSum, maskSums[i]);

          // Assign it back in here
          maxSum[maxSumCounter] = _mm256_srai_epi16(tempMaxSum, 3);

          // Prepare next iteration
          for (int i = 0; i < NUM_ROWS; ++i)
          {
            rows[i]   = nextRows[i];
            rowsX3[i] = nextRowsX3[i];
            rowsX5[i] = nextRowsX5[i];

            // Load the next rows and recompute X3 & X5 (Copy pasted from above)
            nextRows[i] = _mm256_load_si256(++rowPointers[i]);

            // X3
            nextRowsX3[i] = _mm256_sub_epi16(nextRows[i], nextRows[i]);
            for (int j = 0; j < 3; ++j)
              nextRowsX3[i] = _mm256_sub_epi16(nextRowsX3[i], nextRows[i]);

            // X5
            nextRowsX5[i] = _mm256_add_epi16(_mm256_slli_epi16(nextRows[i], 2), nextRows[i]);
          }
        }

        // Pack the max sums together
        __m128i maxSum0Low  = _mm256_extracti128_si256(maxSum[0], 0);
        __m128i maxSum0High = _mm256_extracti128_si256(maxSum[0], 1);
        __m128i maxSum1Low  = _mm256_extracti128_si256(maxSum[1], 0);
        __m128i maxSum1High = _mm256_extracti128_si256(maxSum[1], 1);

        maxSum[0] = _mm256_set_m128i(maxSum1Low, maxSum0Low);
        maxSum[1] = _mm256_set_m128i(maxSum1High, maxSum0High);
        
        _256toUChar converter;
        _mm256_store_si256(&converter.value, _mm256_packus_epi16(maxSum[0], maxSum[1]));

        const int OUTPUT_INDEX = layer * SIZE + row * width + col;
        memcpy_s(&data_out[OUTPUT_INDEX], 32, converter.vector, 32);
      }

      // Clean up: Perform standard un-vectorised kirsch for remaining pixels.
      for (; col < width - 1; ++col)
      {
        int maxSum = 0;
        for (unsigned mask = 0; mask < NUM_MASKS; ++mask) 
        {
          int sum = 0;

          /* Convolution part */
          for (int maskRow = -1; maskRow < 2; ++maskRow)
            for (int maskCol = -1; maskCol < 2; ++maskCol)
            {
              int maskRowIndex = maskRow + 1;
              int maskColIndex = maskCol + 1;

              int incPixelIndex = layer * SIZE + (row + maskRow) * width + (col + maskCol);
              int incPixel = static_cast<int>(data_in[incPixelIndex]);

              sum += KIRSCH_MASKS[mask][maskRowIndex][maskColIndex] * incPixel;
            }

            if (sum > maxSum)
              maxSum = sum;
        }

        maxSum = (maxSum / 8) > 255 ? 255: (maxSum / 8);

        const int OUTPUT_INDEX = layer * SIZE + row * width + col;
        data_out[OUTPUT_INDEX] = maxSum;
      }
    }
  }

  // Free array of colours
  for (int i = 0; i < NUM_LAYERS; ++i)
    std::free(colourRows[i]);
}
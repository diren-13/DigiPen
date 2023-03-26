/************************************************************************/
/*! Start Header
\file main.cpp
\author 
\par email: 
\par DigiPen login: 
\par Course: Low-level Programmming
\par Assignment #2
\date 
\brief
Optimise Kirsch edge detection C code using sse and avx operations
simple driver samples for testing. Kirsch edge detection C code is defined in
edge.cpp.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
***********************************************************************/

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <cstdlib>
#include <immintrin.h> ///AVX
//#include <zmmintrin.h> ///AVX512
#include "bmp.h"
#include "measure.h"
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
  #include <sched.h>
  #define memcpy_s(d, n, s, c) memcpy(d, s, c)
#endif

#include "kirsch.h"
/* just used for time measurements */
#define REP 10
#define MIN(X, Y) (((X)<(Y))? X:Y)

typedef void (*test_func) (unsigned char *, unsigned char *, unsigned, unsigned);


void sobel_edge_detection_avx
(
  unsigned char *data_out
, unsigned char *data_in
, unsigned height
, unsigned width
)
{
  unsigned int size, i, j, lay;

  //Increase to multiple of 32 due to AVX 256 bits
  //Because we are doing unrolling factor 32, best if arrays are in multiple of 32
  //num_of_ints_per_row will be a multiple of 32
  unsigned num_of_ints_per_row = width;
  
  num_of_ints_per_row = (num_of_ints_per_row + 31) / 32 * 32;

  size = height * width;
  unsigned short *color_rows[3];

  /*
  For our sanity's sake, we divide the 3 colours into
  3 separate arrays. Each starting at an address that is 64-byte aligned.
  */

  //Allocate memory based on 64 bytes multiple
  //color_rows[0] is red, color_rows[1] is green, color_rows[2] is blue
  color_rows[0] = static_cast<unsigned short*>(std::aligned_alloc(64, sizeof(unsigned short) * num_of_ints_per_row * height));
  color_rows[1] = static_cast<unsigned short*>(std::aligned_alloc(64, sizeof(unsigned short) * num_of_ints_per_row * height));
  color_rows[2] = static_cast<unsigned short*>(std::aligned_alloc(64, sizeof(unsigned short) * num_of_ints_per_row * height));

  /////////////////////////////////////////////
  //Copy data into the allocated memory
  unsigned char *my_data_temp_in = data_in;
  for (lay = 0; lay < 3; lay++) 
  {
    //Get pointer to each row
    unsigned short *temp_rows = color_rows[lay];

    //Copy data
    for (i = 0; i < height; i++)
      for (j = 0; j < width; j++)
        temp_rows[i * num_of_ints_per_row + j] = my_data_temp_in[i * width + j];

    //Offset to next color group
    my_data_temp_in += size;
  }

  ////////////////////////////////////////////
  typedef union 
  {
    __m256i       value;
    unsigned char vector[32];
  } vec_or_val;

  // Layer count, each time for a color row
  for (lay = 0; lay < 3; lay++) 
  {

    //Now this loop is for the y rows.
    //Start from 1 because we don't do the algorithm on the 0th row
    unsigned short *my_data_in_rmp = color_rows[lay];
    for (i = 1; i < height - 1; ++i) 
    {
      /*
      In order to obtain the correct mask value for each row,
      we need to perform additions or subtractions in parallel.
      Note that we do not need to do multiplications because 1+1=2.

      So we always maintain an upper row, middle row and a lower row.
      Loading 16 pixel values at each time.
      */

      //Get pointers to y-1 (upper row), y (mid row), y+1(bottom row)
      __m256i *upper_row_ptr  = (__m256i *) &my_data_in_rmp[(i - 1) * num_of_ints_per_row];
      __m256i *middle_row_ptr = (__m256i *) &my_data_in_rmp[i * num_of_ints_per_row];
      __m256i *lower_row_ptr  = (__m256i *) &my_data_in_rmp[(i + 1) * num_of_ints_per_row];

      //Load the memory into the row values
      __m256i upper_row, lower_row, middle_row;
      upper_row   = _mm256_load_si256(upper_row_ptr);
      middle_row  = _mm256_load_si256(middle_row_ptr);
      lower_row   = _mm256_load_si256(lower_row_ptr);

      //////////////NOTE!
      //__m256i is 256 bits, which is able to store 32 chars (32*8 = 256)
      //But, because of arithmethic underflow / overflow, the code uses
      //16 shorts instead (16 * 16 = 256). Because of this,
      //the code and comments beneath shows that m256 only has 16 variables

      /*
      In order to perform our optimization, we shall need the following rows

      neg_upper_row = - (upper row) // a negation of all the upper row values

      So if the upper row is u15, u14, u13, u12, u11, u10, u9, u8, u7, u6, u5, u4, u3, u2, u1, u0 (Ordered from right to left)
      neg_upper_row is -u15, -u14, -u13, -u12, -u11, -u10, -u9, -u8, -u7, -u6, -u5, -u4, -u3, -u2, -u1, -u0 (Ordered from right to left)

      twice_upper_row = (upper_row)+(upper_row)

      So if the upper row is u15, u14, u13, u12, u11, u10, u9, u8, u7, u6, u5, u4, u3, u2, u1, u0 (Ordered from right to left)
      neg_upper_row is 2u15, 2u14, 2u13, 2u12, 2u11, 2u10, 2u9, 2u8, 2u7, 2u6, 2u5, 2u4, 2u3, 2u2, 2u1, 2u0 (Ordered from right to left)

      twice_middle_row = (middle_row)+(middle_row)

      So if the middle row is m15, m14, m13, m12, m11, m10, m9, m8, m7, m6, m5, m4, m3, m2, m1, m0 (Ordered from right to left)
      neg_middle_row is 2m15, 2m14, 2m13, 2m12, 2m11, 2m10, 2m9, 2m8, 2m7, 2m6, 2m5, 2m4, 2u3, 2u2, 2u1, 2u0 (Ordered from right to left

      twice_lower_row = (lower_row)+(lower_row)

      So if the lower row is l15, l14, l13, l12, l11, l10, l9, l8, l7, l6, l5, l4, l3, l2, l1, l0 (Ordered from right to left)
      neg_lower_row is 2l15, 2l14, 2l13, 2l12, 2l11, 2l10, 2l9, 2l8, 2l7, 2l6, 2l5, 2l4, 2l3, 2l2, 2l1, 2l0 (Ordered from right to left

      */

      //Double it
      __m256i twice_upper_row   = _mm256_add_epi16(upper_row, upper_row);
      __m256i twice_middle_row  = _mm256_add_epi16(middle_row, middle_row);
      __m256i twice_lower_row   = _mm256_add_epi16(lower_row, lower_row);

      //lower_row + twice_middle_row + upper_row
      __m256i hmask_values = _mm256_add_epi16(upper_row, _mm256_add_epi16(lower_row, twice_middle_row));

      /*
      Putting everything together

      Given:
      u15, u14, u13, u12, u11, u10, u9, u8, u7, u6, u5, u4, u3, u2, u1, u0
      m15, m14, m13, m12, m11, m10, m9, m8, m7, m6, m5, m4, m3, m2, m1, m0
      l15, l14, l13, l12, l11, l10, l9, l8, l7, l6, l5, l4, l3, l2, l1, l0

      The sobel horizontal mask to replace m1 should be:
 
      (u0+2m0+l0)-(u2+2m2+l2)

      Note that this can easily be obtained by doing

      (upper_row + twice_middle_row + lower_row)
      sub
      (upper_row + twice_middle_row + lower_row) shifted right by 32 bits.

      so hmask_values is basically (upper_row + twice_middle_row + lower_row)

      But the problem with the above is that we cannot compute
      the convolution value for m15 since we need the u0, m0 and l0
      of the NEXT iteration. How? we maintain the values of
      the "next" upper_rows, lower_rows and middle_rows in each iteration.
      */

      //Skip to next batch of 16 pixels (each pixel is 16 bits)
      upper_row_ptr++;
      middle_row_ptr++;
      lower_row_ptr++;

      //Load the next row data into variable
      __m256i next_upper_row  = _mm256_load_si256(upper_row_ptr);
      __m256i next_middle_row = _mm256_load_si256(middle_row_ptr);
      __m256i next_lower_row  = _mm256_load_si256(lower_row_ptr);

      //Double it
      __m256i next_twice_upper_row  = _mm256_add_epi16(next_upper_row, next_upper_row);
      __m256i next_twice_middle_row = _mm256_add_epi16(next_middle_row, next_middle_row);
      __m256i next_twice_lower_row  = _mm256_add_epi16(next_lower_row, next_lower_row);

      //Build positive mask
      __m256i next_hmask_values = _mm256_add_epi16(next_upper_row,_mm256_add_epi16(next_lower_row, next_twice_middle_row));

      //j is pixel counter, adds 32 per loop, so it travels per pixel for the whole row
      //Again, it starts from j = 1 beause 0th pixel is ignored
      for (j = 1; j < width - 33; j = j + 32) 
      {
        //printf("remaining j = %d\n", j);
        //This k inner loop is needed, within your x and y loop, because
        //1 __m256i varaible holds the variables for only 16 shorts
        //2 __m256i variables can therefore have 32 shorts, 
        //and we later 'pack' these 2 __m256i variables into 1 variable, taking away the short portion
        __m256i total_sum1, total_sum2;
        for (int k = 0; k < 2; k++) 
        {
          /*
          next_hmask_values has upper_row + twice_middle_row + lower_row
          for  positions 16-31

          we need the values of that for positions 2 - 17

          real_hmask_left holds the values of the positions 2-17
          */
          //Build the left mask that contains data 2 to 17
          //by shifting bits around, 
          //  shift the next mask by 6 px left to get     [17 16 0  0  0  0  0  0  0  0 0 0 0 0 0 0],
          //  shift the right mask by 2 px right to get   [0  0  15 14 13 12 11 10 9  8 7 6 5 4 3 2]
          //  OR together to get                          [17 16 15 14 13 12 11 10 9  8 7 6 5 4 3 2]
          // Now we can add the 2 masks together


          __m128i temp128i_1 = _mm256_extracti128_si256(next_hmask_values, 0x0);//lower 16 bytes
          __m128i temp128i_2 = _mm256_extracti128_si256(hmask_values, 0x0);     //lower 16 bytes
          __m128i temp128i_3 = _mm256_extracti128_si256(hmask_values, 0x1);     //higher 16 bytes


          __m128i temp128i_4 = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
          __m128i temp128i_5 = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
          __m256i hmask_left = _mm256_set_m128i(temp128i_4, temp128i_5);


          /*
          hmask right holds the values of positions 0-15

          then hmask sum holds the convolution result of positions 1-16.
          */

          __m256i hmask_sum = _mm256_sub_epi16(hmask_left, hmask_values);
          hmask_sum = _mm256_srai_epi16(_mm256_abs_epi16(hmask_sum), 3);


          /*
          Now the vertical masks only deals with
          the upper row and the lower row.

          Essentially we should obtain the values
          of ui+2u(i+1)+u(i+2) for positions i:0 to 15
          for upper row
          and
          li+2l(i+1)+l(i+2) for positions i:0 to 15
          for lower row.

          After obtaining those, simply subtract upper row from the lower row.
          */


          /*
          Get upper row values for positions 2 to 17.
          */
          temp128i_1 = _mm256_extracti128_si256(next_upper_row, 0x0);//lower 16 bytes
          temp128i_2 = _mm256_extracti128_si256(upper_row, 0x0); //lower 16 bytes
          temp128i_3 = _mm256_extracti128_si256(upper_row, 0x1); //higher 16 bytes
          temp128i_4 = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
          temp128i_5 = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
          __m256i upper_row_pos_2_17 = _mm256_set_m128i(temp128i_4, temp128i_5);

          /*
          Get twice upper row values for positions 1 to 16.
          */
          temp128i_1 = _mm256_extracti128_si256(next_twice_upper_row, 0x0);
          temp128i_2 = _mm256_extracti128_si256(twice_upper_row, 0x0); //lower 16 bytes
          temp128i_3 = _mm256_extracti128_si256(twice_upper_row, 0x1); //higher 16 bytes
          temp128i_4 = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
          temp128i_5 = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));
          __m256i twice_upper_row_pos_1_16 = _mm256_set_m128i(temp128i_4, temp128i_5);

          /*
          vmask upper is simply
          upper_row
          upper_row_pos_2_to_17
          twice_upper_row_pos_1_to_16
          added up
          */
          __m256i vmask_upper = _mm256_add_epi16(upper_row_pos_2_17, _mm256_add_epi16(twice_upper_row_pos_1_16, upper_row));

          /*
          vmask lower is simply
          lower_row
          lower_row_pos_2_to_17
          twice_lower_row_pos_1_to_16
          added up
          */
          temp128i_1 = _mm256_extracti128_si256(next_lower_row, 0x0);
          temp128i_2 = _mm256_extracti128_si256(lower_row, 0x0); //lower 16 bytes
          temp128i_3 = _mm256_extracti128_si256(lower_row, 0x1); //higher 16 bytes
          temp128i_4 = _mm_or_si128(_mm_slli_si128(temp128i_1, 12), _mm_srli_si128(temp128i_3, 4));
          temp128i_5 = _mm_or_si128(_mm_slli_si128(temp128i_3, 12), _mm_srli_si128(temp128i_2, 4));
          __m256i lower_row_pos_2_17 = _mm256_set_m128i(temp128i_4, temp128i_5);

          temp128i_1 = _mm256_extracti128_si256(next_twice_lower_row, 0x0);
          temp128i_2 = _mm256_extracti128_si256(twice_lower_row, 0x0); //lower 16 bytes
          temp128i_3 = _mm256_extracti128_si256(twice_lower_row, 0x1); //higher 16 bytes
          temp128i_4 = _mm_or_si128(_mm_slli_si128(temp128i_1, 14), _mm_srli_si128(temp128i_3, 2));
          temp128i_5 = _mm_or_si128(_mm_slli_si128(temp128i_3, 14), _mm_srli_si128(temp128i_2, 2));
          __m256i twice_lower_row_pos_1_16 = _mm256_set_m128i(temp128i_4, temp128i_5);

          __m256i vmask_lower = _mm256_add_epi16(lower_row_pos_2_17, _mm256_add_epi16(twice_lower_row_pos_1_16, lower_row));

          /*
          With both lower and upper ready,
          we can compute the vmask_sum. as a subtraction
          of vmask_upper from the vmask_lower
          */
          __m256i vmask_sum = _mm256_sub_epi16(vmask_lower, vmask_upper);
          vmask_sum = _mm256_srai_epi16(_mm256_abs_epi16(vmask_sum), 3);

          if (k == 0)
            total_sum1 = _mm256_add_epi16(hmask_sum, vmask_sum);
          else
            total_sum2 = _mm256_add_epi16(hmask_sum, vmask_sum);


          /*
          Shifting the old next_upper_row into the current upper row for
          the next iteration
          */
          upper_row         = next_upper_row;
          middle_row        = next_middle_row;
          lower_row         = next_lower_row;
          twice_upper_row   = next_twice_upper_row;
          twice_middle_row  = next_twice_middle_row;
          twice_lower_row   = next_twice_lower_row;
          hmask_values      = next_hmask_values;

          upper_row_ptr++;
          middle_row_ptr++;
          lower_row_ptr++;

          next_upper_row        = _mm256_load_si256(upper_row_ptr);
          next_middle_row       = _mm256_load_si256(middle_row_ptr);
          next_lower_row        = _mm256_load_si256(lower_row_ptr);
          next_twice_upper_row  = _mm256_add_epi16(next_upper_row, next_upper_row);
          next_twice_middle_row = _mm256_add_epi16(next_middle_row, next_middle_row);
          next_twice_lower_row  =_mm256_add_epi16(next_lower_row, next_lower_row);
          next_hmask_values     = _mm256_add_epi16(next_upper_row, _mm256_add_epi16(next_lower_row, next_twice_middle_row));
        }

        //This is the packing portion, we compress 2 _m256i into 1 and store it into memory
        //but need to take care of packed 16-bit integers. Refer to instrincis manual.
        __m128i total_sum1_low  = _mm256_extracti128_si256(total_sum1, 0x0);//low
        __m128i total_sum1_hi   = _mm256_extracti128_si256(total_sum1, 0x1);//high
        __m128i total_sum2_low  = _mm256_extracti128_si256(total_sum2, 0x0);//low
        __m128i total_sum2_hi   = _mm256_extracti128_si256(total_sum2, 0x1);//high
        total_sum1 = _mm256_set_m128i(total_sum2_low, total_sum1_low);
        total_sum2 = _mm256_set_m128i(total_sum2_hi, total_sum1_hi);

        __m256i total_sum = _mm256_packus_epi16(total_sum1, total_sum2);

        vec_or_val real_sum;
        _mm256_store_si256(&real_sum.value, total_sum);

        memcpy_s(&data_out[lay * size + i * width + j], 32, real_sum.vector, 32);
      }

      //This is the clean up part! 
      //The image size might not be multiple of 16, so we need to deal with the remaining pixels manually.
      //Notice that this code here is pretty much copy pasted from the basis algorithm

      int sobelv[3][3] = 
      { 
        {-1, -2, -1 }, 
        { 0,  0,  0 }, 
        { 1,  2,  1 } 
      };

      int sobelh[3][3] = 
      { 
        {-1,  0,  1 }, 
        {-2,  0,  2 }, 
        {-1,  0,  1 } 
      };

      for (; j < width - 1; j++) 
      {
        int sumh, sumv;
        sumh = 0;
        sumv = 0;

        /* Convolution part */
        for (int k = -1; k < 2; k++)
          for (int l = -1; l < 2; l++) 
          {
            sumh = sumh + sobelh[k + 1][l + 1] * (int)data_in[lay * size + (i + k) * width + (j + l)];
            sumv = sumv + sobelv[k + 1][l + 1] * (int)data_in[lay * size + (i + k) * width + (j + l)];
          }

        int temp = abs(sumh / 8) + abs(sumv / 8);
        data_out[lay * size + i * width + j] = (temp > 255 ? 255 : temp);
      }
    }

  }

  for (int i=0; i<3; i++)
    std::free(color_rows[i]);
}

void kirsch_operator_basic
(
  unsigned char *data_out
, unsigned char *data_in
, unsigned height
, unsigned width
)
{
  /* Kirsch matrices for convolution */
  static int kirsch[8][3][3] = 
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

  unsigned int size, y, x, lay;

  size = height * width;

  for (lay = 0; lay < 3; lay++) 
    for (y = 1; y < height - 1; ++y) 
      for (x = 1; x < width - 1; x++) 
      {
        int maxSum = 0;

        /*
          Perform convolutions for 
          all 8 masks in succession. Compare and find the one
          that has the highest value. The one with the
          highest value is stored into the final bitmap.
        */
        for (unsigned mask = 0; mask < 8; ++mask) 
        {
          int sum = 0;

          /* Convolution part */
          for (int maskRow = -1; maskRow < 2; ++maskRow)
            for (int maskCol = -1; maskCol < 2; ++maskCol)
            {
              int maskRowIndex = maskRow + 1;
              int maskColIndex = maskCol + 1;

              int incPixelIndex = lay * size + (y + maskRow) * width + (x + maskCol);
              int incPixel = static_cast<int>(data_in[incPixelIndex]);

              sum += kirsch[mask][maskRowIndex][maskColIndex] * incPixel;
            }

            if (sum > maxSum)
              maxSum = sum;
        }

        maxSum = (maxSum / 8) > 255 ? 255: (maxSum / 8);

        const int OUTPUT_INDEX = lay * size + y * width + x;
        data_out[OUTPUT_INDEX] = maxSum;
    }
}


void sobel_edge_detection_basic
(
  unsigned char *data_out
, unsigned char *data_in
, unsigned height
, unsigned width
)
{
  /* Sobel matrices for convolution */
  int sobelv[3][3] = 
  { 
    {-1, -2, -1 }, 
    { 0,  0,  0 }, 
    { 1,  2,  1 } 
  };

  int sobelh[3][3] = 
  { 
    {-1,  0,  1 }, 
    {-2,  0,  2 }, 
    {-1,  0,  1 } 
  };

  unsigned int size, i, j, lay;

  size = height * width;

  for (lay = 0; lay < 3; lay++)
    for (i = 1; i < height - 1; ++i)
      for (j = 1; j < width - 1; j++) 
      {
        int sumh, sumv;
        sumh = 0;
        sumv = 0;

        /* Convolution part */
        for (int k = -1; k < 2; k++)
          for (int l = -1; l < 2; l++) 
          {
            int maskRowIndex = k + 1;
            int maskColIndex = l + 1;

            int incPixelIndex = lay * size + (i + k) * width + (j + l);
            int incPixel = static_cast<int>(data_in[incPixelIndex]);

            sumh = sumh + sobelh[maskRowIndex][maskColIndex] * incPixel;
            sumv = sumv + sobelv[maskRowIndex][maskColIndex] * incPixel;
          }
          
        int temp = abs(sumh / 8) + abs(sumv / 8);
        data_out[lay * size + i * width + j] = (temp > 255 ? 255 : temp);
      }
}


int main(int argc, char **argv)
{
  /* Some variables */
  bmp_header header;
  unsigned char *data_in, *data_out;
  unsigned int size;
  int rep;

  test_func functions[4] =
  { 
    sobel_edge_detection_basic, sobel_edge_detection_avx, 
    kirsch_operator_basic,      kirsch_operator_avx
  };
  
  if (argc != 4) 
  {
    printf("Usage: edge <InFile> <OutFile> <0: sobel_basic, 1: sobel_avx, 2: kirsch_basic, 3: kirsch_avx>\n\n");
    exit(0);
  }

  bmp_read(argv[1], &header, &data_in);

  /* Size of image and output buffer */
  size = header.height * header.width;
  data_out = new unsigned char[3 * size];
  memcpy_s(data_out, 3 * size, data_in, 3 * size);
  printf("Resolution: (%d,%d) -> Size: %d\n", header.height, header.width, size);
  prof_time_t start = 0, end = 0;
  start_measure(start);
  sleep(1);
  end_measure(end);

  prof_time_t num_of_cycles_per_sec = end - start;

  int which_func = atoi(argv[3]);
  start_measure(start);
  end_measure(end);

  prof_time_t base_time = end - start;
  start_measure(start);
  for (rep = 0; rep < REP; rep++) 
  {
    functions[which_func] (data_out, data_in, header.height, header.width);
  }
  end_measure(end);

  prof_time_t cycles_taken = end - start - base_time;

  std::cout.setf(std::ios::fixed);
  std::cout.precision(4);
  std::cout << "Time taken: "
            << (cycles_taken / REP) / ((double) num_of_cycles_per_sec)
            << "seconds" << std::endl;

  bmp_write(argv[2], &header, data_out);

  return (0);
}

#include <iostream>
#include <math.h>
#include <string>
#include <cstdlib>
#include <immintrin.h> //AVX

union Converter
{
  __m256i value;
  short   _16bitValues[16];
};

int main()
{

  short nA[16] = { 0 };
  for (int i = 0; i < 16; ++i)
  {
    nA[i] = i;
    std::cout << nA[i] << " ";
  }
  std::cout << std::endl;

  // short n1[16] = { 1 };
  // __m256i b = _mm256_load_si256((__m256i*)n1);
  __m256i a = _mm256_load_si256((__m256i*)nA);

  Converter c;
  _mm256_store_si256(&c.value, a);

  for (int i = 0; i < 16; ++i)
    c._16bitValues[i] = -3 * c._16bitValues[i];

  a = c.value;

  // a = _mm256_sub_epi16(_mm256_add_epi16(~a, b), b);

  _mm256_store_si256((__m256i*)nA, a);

  for (int i = 0; i < 16; ++i)
    std::cout << nA[i] << " ";
  std::cout << std::endl;
}
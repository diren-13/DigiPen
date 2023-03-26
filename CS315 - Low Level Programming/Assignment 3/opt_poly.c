/****************************************************************************************
 * \file      opt_poly.c
 * \author    Diren D Bharwani, diren.dbharwani, 390002520
 * \brief     Implementation for an optimised function to compute the result of a polynomial.
 *
 * \copyright Copyright (C) 2023 DigiPen Institute of Technology. Reproduction or
 *            disclosure of this file or its contents without the prior written consent
 *            of DigiPen Institute of Technology is prohibited.
****************************************************************************************/

#define UNROLL_FACTOR  8

double poly_opt(double* a,double x, long degree)
{
  // Compute powers
  const double x2 = x   * x;
  const double x3 = x2  * x;
  const double x4 = x3  * x;
  const double x5 = x4  * x;
  const double x6 = x5  * x;
  const double x7 = x6  * x;
  const double x8 = x7  * x;

  double result = a[degree];

  long i = degree - 1;
  for (; i >= UNROLL_FACTOR - 1; i -= UNROLL_FACTOR)
  {
    result = 
    (
              a[i - 7]
      + x   * a[i - 6]
      + x2  * a[i - 5]
      + x3  * a[i - 4]
      + x4  * a[i - 3]
      + x5  * a[i - 2]
      + x6  * a[i - 1]
      + x7  * a[i]
    ) + x8 * result;
  }
    
  for (; i >= 0; --i)
    result = a[i] + x * result;

  return result;
}
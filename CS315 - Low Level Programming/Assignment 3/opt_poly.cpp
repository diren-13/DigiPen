/****************************************************************************************
 * \file      poly.cpp
 * \author    Diren D Bharwani, diren.dbharwani, 390002520
 * \brief     Implementation for functions to compute a polynomial.
 *
 * \copyright Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 *            disclosure of this file or its contents without the prior written consent
 *            of DigiPen Institute of Technology is prohibited.
****************************************************************************************/

#include "opt_poly.hpp"

namespace Math
{
  std::default_random_engine RNG::rng;

  double poly(double* a, double x, long degree)
  {
    double result = a[0];
    double xPower = x;      // x^i at start of loop

    for (long i = 1; i <= degree; ++i)
    {
      result += a[i] * xPower;
      xPower *= x;
    }

    return result;
  }

  double polyh(double* a, double x, long degree)
  {
    double result = a[degree];
    for (long i = degree - 1; i >= 0; --i)
      result = a[i] + x * result;

    return result;
  }

  double poly_opt(double* a,double x, long degree)
  {
    static constexpr long FACTOR = 8;

    // Compute powers
    const double x2 = x * x;
    const double x3 = x2 * x;
    const double x4 = x3 * x;
    const double x5 = x4 * x;
    const double x6 = x5 * x;
    const double x7 = x6 * x;
    const double x8 = x7 * x;

    double result = a[degree];
    long i = degree - 1;

    for (; i >= FACTOR - 1; i -= FACTOR)
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
}



/****************************************************************************************
 * \file      poly.hpp
 * \author    Diren D Bharwani, diren.dbharwani, 390002520
 * \brief     Interface for functions to compute a polynomial.
 *
 * \copyright Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 *            disclosure of this file or its contents without the prior written consent
 *            of DigiPen Institute of Technology is prohibited.
****************************************************************************************/

#include <type_traits>
#include <chrono>
#include <numeric>
#include <random>

namespace Math
{
  [[maybe_unused]] double poly     (double* a, double x, long degree);
  [[maybe_unused]] double polyh    (double* a, double x, long degree);
  [[maybe_unused]] double poly_opt (double* a, double x, long degree);

  class RNG
  {
  public:
    static inline void init()
    {
      const unsigned SEED = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
      rng.seed(SEED);
    }
    
    template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    static T generateNumber(T lowerBound = 0, T upperBound = 1)
    {
      if (lowerBound > upperBound)
        std::swap(lowerBound, upperBound);

      if constexpr (std::is_integral_v<T>)
      {
          std::uniform_int_distribution<T> distribution(lowerBound, upperBound);
          return distribution(rng);
      }
      if constexpr (std::is_floating_point_v<T>)
      {
          std::uniform_real_distribution<T> distribution(lowerBound, upperBound);
          return distribution(rng);
      }
    }

  private:
    static std::default_random_engine rng;
  };
}
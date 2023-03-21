/************************************************************************************//*!
\file           CCMath.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains declarations of additional methods for mathematical computations
                which are not found in STL or DirectXTK Headers.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Standard Libraries
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <numeric>
// DirectXTK Headers
#include <SimpleMath.h>

namespace ClamChowder::Math
{
    /*---------------------------------------------------------------------------------*/
    /* Static Global Constants                                                         */
    /*---------------------------------------------------------------------------------*/
    static constexpr float  EPSILON = 0.001f;

    /*---------------------------------------------------------------------------------*/
    /* Global Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr T Infinity();

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr T Radians(T angle);

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr T Degrees(T angle);

    template <typename T = double, typename = std::enable_if_t<std::is_floating_point_v<T>>>
    bool CompareFloat(T a, T b, T absTol = EPSILON, T relTol = EPSILON);

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    T Wrap(T value, T min, T max);
}

#include "CCMath.hpp"
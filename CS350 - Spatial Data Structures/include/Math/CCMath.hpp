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

// Precompiled Header
#include "pch.h"
// Primary Header
#include "CCMath.h"

namespace ClamChowder::Math
{
    /*---------------------------------------------------------------------------------*/
    /* Global Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    template <typename T, typename Condition>
    constexpr T Infinity()
    {
        return std::numeric_limits<T>::infinity();
    }

    template <typename T, typename Condition>
    constexpr T Radians(T angle)
    {
        return angle * static_cast<T>(DirectX::XM_PI / 180.0f);
    }

    template <typename T, typename Condition>
    constexpr T Degrees(T angle)
    {
        return angle * static_cast<T>(180.0f / DirectX::XM_PI);
    }

    template <typename T, typename Condition>
    bool CompareFloat(T a, T b, T absTol, T relTol)
    {
        const T RELATIVE_TOLERANCE  = relTol * (abs(a) > abs(b) ? abs(a) : abs(b));
        const T MAX_TOLERANCE       = absTol > RELATIVE_TOLERANCE ? absTol : RELATIVE_TOLERANCE;
        const T DIFF                = abs(a - b);
        return DIFF <= MAX_TOLERANCE;
    }

    template <typename T, typename Condition>
    T Wrap(T value, T min, T max)
    {
        if (value < min)
        {
            value = max;
        }
        if (value > max)
        {
            value = min;
        }

        return value;
    }

}

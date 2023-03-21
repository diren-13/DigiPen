/************************************************************************************//*!
\file           Ray.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the implementation for the Ray object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Geometry/Ray.h"
// Project Headers
#include "Math/CCMath.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Ray::Ray()
    : position  { Vec3::Zero }
    , direction { Vec3::Right }
    , t         { std::numeric_limits<float>::infinity() }
    {}

    Ray::Ray(const Vec3& pos, const Vec3& dir)
    : position  { pos }
    , direction { dir }
    , t         { std::numeric_limits<float>::infinity() }
    {
        if (dir.Length() > 1.0f)
        {
            dir.Normalize(direction);
        }
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Ray::SetPosition(const Vec3& pos)
    {
        position = pos;
    }

    void Ray::SetDirection(const Vec3& dir)
    {
        direction = dir;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void Ray::LookAt(const Vec3& target)
    {
        (target - position).Normalize(direction);
    }
}
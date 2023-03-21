/************************************************************************************//*!
\file           Transform.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Implementation for a Transform.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Math/Transform.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors Definition                                                         */
    /*---------------------------------------------------------------------------------*/

    Transform::Transform()
    : dirty     { true }
    , position  { Vec3::Zero }
    , rotation  { Vec3::Zero }
    , scale     { Vec3::One }
    {
        Update();
    }

    Transform::Transform(const Vec3& pos, const Vec3& rot, const Vec3& scl)
    : dirty     { true }
    , position  { pos }
    , rotation  { rot }
    , scale     { scl }
    {
        Update();
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void Transform::Update()
    {
        if (dirty)
        {
            Mat4 T = Mat4::CreateTranslation(position);
            Mat4 R = Mat4::CreateFromYawPitchRoll(rotation);
            Mat4 S = Mat4::CreateScale(scale);
            TRS =  S * R * T;

            dirty = false;
        }
    }
}
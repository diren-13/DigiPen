/************************************************************************************//*!
\file           MathDefines.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the defines, typedefs or type aliases for commonly used
                mathematical objects.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// DirectXTK Headers
#include <SimpleMath.h>
#include <DirectXColors.h>

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    using Vec2          =   DirectX::SimpleMath::Vector2;
    using Vec3          =   DirectX::SimpleMath::Vector3;
    using Vec4          =   DirectX::SimpleMath::Vector4;
    using Mat4          =   DirectX::SimpleMath::Matrix;
    using Quaternion    =   DirectX::SimpleMath::Quaternion;
    using Colour        =   DirectX::SimpleMath::Color;

    namespace  Colours  =  DirectX::Colors;
}

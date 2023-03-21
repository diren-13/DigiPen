/************************************************************************************//*!
\file           Light.hpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 16, 2022
\brief          Implementation of a Light. Referenced from
                https://github.com/nitvic793/rendering-engine/blob/master/RenderingEngine/Lights.h
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Primary Header
#include "Light.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    template <typename LightType>
    LightType* Light::GetLight()
    {
        switch (type)
        {
            case Type::Directional: return static_cast<DirectionalLight*>(instance);
            //case Type::Point:       return static_cast<PointLight*>(instance);
            default:                return nullptr;
        }
    }

    template <typename LightType>
    const LightType* Light::GetLight() const
    {
        switch (type)
        {
            case Type::Directional: return static_cast<const DirectionalLight*>(instance);
            //case Type::Point:       return static_cast<const PointLight*>(instance);
            default:                return nullptr;
        }
    }


}

/************************************************************************************//*!
\file           Light.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 16, 2022
\brief          Implementation of a Light.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Light.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Light::Light(Type t)
    : type { t }
    {
        switch (type)
        {
            case Type::Directional: instance = new DirectionalLight{}; break;
            //case Type::Point:
            default:                instance = nullptr; break;
        }
    }

    Light::Light(const DirectionalLight& dirLight)
    : type { Type::Directional }
    {
        instance = new DirectionalLight{};

        auto* light = GetLight<DirectionalLight>();

        light->AmbientColour    = dirLight.AmbientColour;
        light->AmbientIntensity = dirLight.AmbientIntensity;
        light->DiffuseColour    = dirLight.DiffuseColour;
        light->Direction        = dirLight.Direction;
    }

    Light::Light(const Light& rhs)
    : type { rhs.type }
    {
        switch (type)
        {
            case Type::Directional: 
            {
                instance = new DirectionalLight{};

                auto* rhsLight = rhs.GetLight<DirectionalLight>();
                auto* dirLight = GetLight<DirectionalLight>();

                dirLight->AmbientColour     = rhsLight->AmbientColour;
                dirLight->AmbientIntensity  = rhsLight->AmbientIntensity;
                dirLight->DiffuseColour     = rhsLight->DiffuseColour;
                dirLight->Direction         = rhsLight->Direction;

                break;
            }
            default:
            {
                instance = nullptr;
                break;
            }
        }
    }

    Light& Light::operator=(const Light& rhs)
    {
        type = rhs.type;

        switch (type)
        {
            case Type::Directional: 
            {
                instance = new DirectionalLight{};

                auto* rhsLight = rhs.GetLight<DirectionalLight>();
                auto* dirLight = GetLight<DirectionalLight>();

                dirLight->AmbientColour     = rhsLight->AmbientColour;
                dirLight->AmbientIntensity  = rhsLight->AmbientIntensity;
                dirLight->DiffuseColour     = rhsLight->DiffuseColour;
                dirLight->Direction         = rhsLight->Direction;

                break;
            }
            default:
            {
                instance = nullptr;
                break;
            }
        }

        return *this;
    }

    Light::~Light()
    {
        delete instance;
    }


}
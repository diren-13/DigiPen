/************************************************************************************//*!
\file           Light.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 16, 2022
\brief          Encapsulation of a Light. Referenced from
                https://github.com/nitvic793/rendering-engine/blob/master/RenderingEngine/Lights.h
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Math/MathDefines.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class ILight{};

    struct alignas(16) DirectionalLight : ILight
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Colour  AmbientColour;
        Colour  DiffuseColour;
        Vec3    Direction;
        float   AmbientIntensity = 1.0f;
    };

    class Light
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        enum class Type
        {
            Directional
        ,   Point           // TODO
        };

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr unsigned MAX_LIGHTS = 32;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Light           (Type type);
        Light           (const DirectionalLight& dirLight);
        Light           (const Light& rhs);
        Light& operator=(const Light& rhs);
        ~Light          ();

        /*-----------------------------------------------------------------------------*/
        /* Getter Function                                                             */
        /*-----------------------------------------------------------------------------*/
        template <typename LightType = DirectionalLight> [[nodiscard]] LightType*       GetLight();
        template <typename LightType = DirectionalLight> [[nodiscard]] const LightType* GetLight() const;
        [[nodiscard]] Type                                                              GetType () const { return type; }

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        ILight* instance;
        Type    type;

        
    };
}

#include "Light.hpp"
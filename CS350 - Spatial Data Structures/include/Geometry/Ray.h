/************************************************************************************//*!
\file           Ray.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the interface for the Ray object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Math/MathDefines.h"

/*-------------------------------------------------------------------------------------*/
/* Forward Declarations                                                                */
/*-------------------------------------------------------------------------------------*/
namespace ClamChowder
{
    class Scene;
}

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class Sphere;
    class AABB;
    class Plane;
    class Triangle;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    class Ray
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Ray ();
        Ray (const Vec3& pos, const Vec3& dir);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const Vec3&   GetPosition ()  const   { return position; }
        [[nodiscard]] const Vec3&   GetDirection()  const   { return direction; }
        [[nodiscard]] float         GetT        ()  const   { return t; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetPosition (const Vec3& pos);
        void    SetDirection(const Vec3& dir);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
        @brief      Sets the direction of the ray towards the target point.

        @param[in]  target
            The target to look at.
        *//*****************************************************************************/
        void    LookAt  (const Vec3& target);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Vec3    position;
        Vec3    direction;
        float   t;

        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class ClamChowder::Scene;
        friend class Sphere;
        friend class AABB;
        friend class Plane;
        friend class Triangle;
    };

}

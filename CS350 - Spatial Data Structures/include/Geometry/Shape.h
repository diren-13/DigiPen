/************************************************************************************//*!
\file           Shape.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the interface for the Shape object, which is the base object
                used to represent bounding volumes.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
// Project Headers
#include "Math/MathDefines.h"
#include "Math/CCMath.h"
#include "Math/Transform.h"
#include "Ray.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Shape
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        enum class Type
        {
            // TODO: OBB, Ellipsoid, kDOP, ConvexHull
            Sphere
        ,   AABB
        ,   Plane
        ,   Triangle

        ,   Total
        ,   None        = -1
        };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Shape           () = default;
        virtual ~Shape  () = default;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] Type GetType          ()  const   { return type; }
        [[nodiscard]] bool GetIntersecting  ()  const   { return intersecting; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetIntersecting(bool isIntersecting) { intersecting = isIntersecting; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        virtual void                Update          (const Transform& tf)                                       = 0;
        [[nodiscard]] virtual bool  TestPoint       (const Vec3& point, float e = Math::EPSILON)                = 0;
        [[nodiscard]] virtual bool  Raycast         (Ray& ray)                                                  = 0;
        [[nodiscard]] static Vec3   GetSupportPoint (const std::vector<Vec3>& vertices, const Vec3& direction);
        [[nodiscard]] static Vec3   GetSupportPoint (const Vec3* vertices, int numVertices, const Vec3& direction);

    protected:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Type type           = Type::None;
        bool intersecting   = false;
    };
}


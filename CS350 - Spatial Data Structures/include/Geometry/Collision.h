/************************************************************************************//*!
\file           Collision.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Contains the interface for methods used to detect collisions between
                two bounding volumes.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Shape.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class CollisionController
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        static bool Collide         (Geometry::Shape& a, Geometry::Shape& b);

        /// Region-Based
        static bool SphereVSSphere  (Geometry::Shape& a, Geometry::Shape& b);
        static bool SphereVSAABB    (Geometry::Shape& a, Geometry::Shape& b);
        static bool AABBVSSphere    (Geometry::Shape& a, Geometry::Shape& b);
        static bool AABBVSAABB      (Geometry::Shape& a, Geometry::Shape& b);

        /// Plane-Based
        static bool SphereVSPlane   (Geometry::Shape& a, Geometry::Shape& b);
        static bool PlaneVSSphere   (Geometry::Shape& a, Geometry::Shape& b);
        static bool AABBVSPlane     (Geometry::Shape& a, Geometry::Shape& b);
        static bool PlaneVSAABB     (Geometry::Shape& a, Geometry::Shape& b);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using CollideFunction = bool(*)(Geometry::Shape&, Geometry::Shape&);

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static CollideFunction Jump[3][3];

    };

    
}
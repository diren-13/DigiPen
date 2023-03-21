/************************************************************************************//*!
\file           Plane.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the interface for the Plane object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Shape.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    class Plane final : public Shape
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Plane               ();
        Plane               (const Plane& rhs);
        Plane& operator=    (const Plane& rhs);
        Plane               (Plane&& rhs) noexcept;
        Plane& operator=    (Plane&& rhs) noexcept;

        Plane               (const Vec3& n, float d);
        Plane               (const Vec3& n, const Vec3& p);
        //Plane               (const Vec3& p1, const Vec3& p2, const Vec3& p3);
        explicit Plane      (const Vec4& v);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] Vec3      GetNormal   ()  const noexcept;
        [[nodiscard]] float     GetDistance ()  const noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetNormal   (const Vec3& n) noexcept;
        void    SetDistance (float d)       noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Operators                                                                   */
        /*-----------------------------------------------------------------------------*/
        bool    operator==(const Plane& rhs) const noexcept;
        bool    operator!=(const Plane& rhs) const noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] void Update       (const Transform& tf)                           override;
        [[nodiscard]] bool TestPoint    (const Vec3& point, float e = Math::EPSILON)    override;
        [[nodiscard]] bool Raycast      (Ray& ray)                                      override;

        [[nodiscard]] bool IsInFront    (const Vec3& point) const;
        [[nodiscard]] bool IsBehind     (const Vec3& point) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Vec4 planeEquation;     // n.x, n.y, n.z, d
    };
}


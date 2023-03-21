/************************************************************************************//*!
\file           Plane.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the implementation for the Plane object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Geometry/Plane.h"
// Project Headers
#include "Math/CCMath.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    Plane::Plane()
    : planeEquation { 0.0f, 1.0f, 0.0f, 0.0f }
    {
        type = Type::Plane;
    }

    Plane::Plane(const Vec3& n, float d)
    : planeEquation { n.x, n.y, n.z, d }
    {
        Vec3 normal = { n.x, n.y, n.z };
        // Normalise if not already
        if (normal.Length() < 1.0f)
        {
            normal.Normalize();
            planeEquation.x = normal.x;
            planeEquation.y = normal.y;
            planeEquation.z = normal.z;
        }

        type = Type::Plane;
    }

    Plane::Plane(const Vec3& n, const Vec3& p)
    : planeEquation { n.x, n.y, n.z, 0.0f }
    {
        Vec3 normal = { n.x, n.y, n.z };
        // Normalise if not already
        if (normal.Length() < 1.0f)
        {
            normal.Normalize();
            planeEquation.x = normal.x;
            planeEquation.y = normal.y;
            planeEquation.z = normal.z;
        }
        
        planeEquation.w = normal.Dot(p);

        type = Type::Plane;
    }

    Plane::Plane(const Vec4& v)
    : planeEquation { v }
    {
        type = Type::Plane;
    }

    Plane::Plane(const Plane& rhs)
    : planeEquation { rhs.planeEquation }
    {
        type = Type::Plane;
    }

    Plane& Plane::operator=(const Plane& rhs)
    {
        planeEquation = rhs.planeEquation;
        return *this;
    }

    Plane::Plane(Plane&& rhs) noexcept
    : planeEquation { rhs.planeEquation }
    {
        type = Type::Plane;
    }

    Plane& Plane::operator=(Plane&& rhs) noexcept
    {
        planeEquation = rhs.planeEquation;
        return *this;
    }

    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    Vec3 Plane::GetNormal() const noexcept
    {
        return Vec3{ planeEquation.x, planeEquation.y, planeEquation.z };
    }

    float Plane::GetDistance() const noexcept
    {
        return planeEquation.w;
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Plane::SetNormal(const Vec3& n) noexcept
    {
        Vec3 normal = n;

        // Normalise if not already
        if (normal.Length() < 1.0f)
        {
            normal.Normalize();
        }

        planeEquation.x = normal.x;
        planeEquation.y = normal.y;
        planeEquation.z = normal.z;
    }

    void Plane::SetDistance(float d) noexcept
    {
        planeEquation.w = -d;
    }

    /*---------------------------------------------------------------------------------*/
    /* Operator Definitions                                                            */
    /*---------------------------------------------------------------------------------*/

    inline bool Plane::operator==(const Plane& rhs) const noexcept
    {
        return planeEquation == rhs.planeEquation;
    }

    inline bool Plane::operator!=(const Plane& rhs) const noexcept
    {
        return planeEquation != rhs.planeEquation;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/
    void Plane::Update(const Transform&)
    {
        intersecting = false;
    }

    bool Plane::TestPoint(const Vec3& point, float e)
    {
        const float R = GetNormal().Dot(point) - planeEquation.w;
        const bool TEST = Math::CompareFloat(R, 0.0f, e, e);
        if (TEST)
        {
            intersecting = true;
        }
        return TEST;
    }

    bool Plane::Raycast(Ray& ray)
    {
        const Vec3& P = ray.GetPosition();
        const Vec3& D = ray.GetDirection();

        const Vec3 N = GetNormal();

        // Check if ray is parallel to plane
        const float N_DOT_D = N.Dot(D);
        if (Math::CompareFloat(N_DOT_D, 0.0f))
            return false;

        ray.t = (planeEquation.w - N.Dot(P)) / N_DOT_D;
        const bool TEST = ray.t > 0.0f || Math::CompareFloat(ray.t, 0.0f);
        if (TEST)
        {
            intersecting = true;
        }
        return TEST;
    }

    bool Plane::IsInFront(const Vec3& point) const
    {
        return (GetNormal().Dot(point) - planeEquation.w) > 0.0f;
    }

    bool Plane::IsBehind(const Vec3& point) const
    {
        return (GetNormal().Dot(point) - planeEquation.w) < 0.0f;
    }

}
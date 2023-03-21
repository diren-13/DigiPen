/************************************************************************************//*!
\file           Triangle.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the implementation for the Triangle object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Geometry/Triangle.h"
// Project Headers
#include "Math/CCMath.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Triangle::Triangle()
    : v0            { Vec3::Left }
    , v1            { Vec3::Forward }
    , v2            { Vec3::Down }
    , signedArea2   { ((v1 - v0).Cross(v2 - v0)).Length() }
    {
        type = Type::Triangle;
    }

    Triangle::Triangle(const Triangle& rhs)
    : v0            { rhs.v0 }
    , v1            { rhs.v1 }
    , v2            { rhs.v2 }
    , signedArea2   { rhs.signedArea2 }
    {
        type = Type::Triangle;
    }

    Triangle::Triangle(Triangle&& rhs) noexcept
    : v0            { rhs.v0 }
    , v1            { rhs.v1 }
    , v2            { rhs.v2 }
    , signedArea2   { rhs.signedArea2 }
    {
        type = Type::Triangle;
    }

    Triangle::Triangle(const Vec3& vertex1, const Vec3& vertex2, const Vec3& vertex3)
    : v0            { vertex1 }
    , v1            { vertex2 }
    , v2            { vertex3 }
    , signedArea2   { ((v1 - v0).Cross(v2 - v0)).Length() }
    {
        // Check if orientation is CW
        //if (signedArea2 < 0.0f)
        //{
        //    // Flip v1 & v2 to set to CCW orientation
        //    std::swap(v1, v2);
        //    // Recompute signed area
        //    signedArea2 = ((v1 - v0).Cross(v2 - v0)).Length();
        //}

        type = Type::Triangle;
    }

    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/
    Vec3 Triangle::GetEdge1() const
    {
        return v2 - v1;
    }

    Vec3 Triangle::GetEdge2() const
    {
        return v0 - v2;
    }

    Vec3 Triangle::GetEdge3() const
    {
        return v1 - v0;
    }


    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void Triangle::Update(const Transform&)
    {
        intersecting = false;
    }

    bool Triangle::TestPoint(const Vec3& point, float e)
    {
        const float INV_2_SIGNED_AREA = inv2SignedArea();

        const float ALPHA   = ((v1 - point).Cross(v2 - point)).Length() * INV_2_SIGNED_AREA;
        const float BETA    = ((v2 - point).Cross(v0 - point)).Length() * INV_2_SIGNED_AREA;
        const float GAMMA   = ((v0 - point).Cross(v1 - point)).Length() * INV_2_SIGNED_AREA;

        const bool CA = ALPHA >= 0.0f && ALPHA <= 1.0f;
        const bool CB = BETA >= 0.0f && BETA <= 1.0f;
        const bool CG = GAMMA >= 0.0f && GAMMA <= 1.0f;
        const bool C1 = Math::CompareFloat(ALPHA + BETA + GAMMA, 1.0f, e, e);

        const bool TEST = CA && CB && CG && C1;
        if (TEST)
        {
            intersecting = true;
        }

        return TEST;
    }

    bool Triangle::Raycast(Ray& ray)
    {
        const Vec3 AB   = (v1 - v0);
        const Vec3 AC   = (v2 - v0);

        const Vec3 PVEC = ray.GetDirection().Cross(AC);
        const float DET =AB.Dot(PVEC);
        if (fabs(DET) <= Math::EPSILON)
            return false;

        const float INV_DET = 1.0f / DET;

        const Vec3 TVEC = ray.GetPosition() - v0;

        const float U = TVEC.Dot(PVEC) * INV_DET;
        if (U < 0.0f || U > 1.0f)
            return false;

        const Vec3 QVEC = TVEC.Cross(AB);
        const float V = ray.GetDirection().Dot(QVEC) * INV_DET;
        if (V < 0.0f || U + V > 1.0f)
            return false;

        ray.t = AC.Dot(QVEC) * INV_DET;

        intersecting = true;
        return true;
    }




    /*---------------------------------------------------------------------------------*/
    /* Private Function Member Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    float Triangle::inv2SignedArea() const
    {
        return (Math::CompareFloat(signedArea2, 0.0f) ? 0.0f : (1.0f / signedArea2));
    }

}
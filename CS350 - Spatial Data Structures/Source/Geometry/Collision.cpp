/************************************************************************************//*!
\file           Collision.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Contains the implementation for methods used to detect collisions between
                two bounding volumes.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Geometry/Collision.h"
// Project Headers
#include "Math/CCMath.h"
#include "Geometry/Sphere.h"
#include "Geometry/AABB.h"
#include "Geometry/Plane.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/

    CollisionController::CollideFunction CollisionController::Jump[3][3] =
    {
        { SphereVSSphere,   SphereVSAABB,   SphereVSPlane   },
        { AABBVSSphere,     AABBVSAABB,     AABBVSPlane     },
        { PlaneVSSphere,    PlaneVSAABB,    nullptr         }
    };

    /*---------------------------------------------------------------------------------*/
    /* Global Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    bool CollisionController::Collide(Geometry::Shape& A, Geometry::Shape& B)
    {
        const int TYPE_A = static_cast<int>(A.GetType());
        const int TYPE_B = static_cast<int>(B.GetType());

        return Jump[TYPE_A][TYPE_B](A, B);
    }


    bool CollisionController::SphereVSSphere(Geometry::Shape& A, Geometry::Shape& B)
    {
        auto& a = reinterpret_cast<Geometry::Sphere&>(A);
        auto& b = reinterpret_cast<Geometry::Sphere&>(B);

        const Vec3 AC = a.GetCenter();
        const Vec3 BC = b.GetCenter();

        const float D_SQ    = Vec3::DistanceSquared(AC, BC);
        const float CR_SQ   = (a.GetRadius() + b.GetRadius()) * (a.GetRadius() + b.GetRadius());

        const bool RESULT = D_SQ < CR_SQ || Math::CompareFloat(D_SQ - CR_SQ, 0.0f);
        if (RESULT)
        {
            A.SetIntersecting(true);
            B.SetIntersecting(true);
        }

        return RESULT;
    }

    bool CollisionController::SphereVSAABB(Geometry::Shape& A, Geometry::Shape& B)
    {
        auto& a = reinterpret_cast<Geometry::Sphere&>(A);
        auto& b = reinterpret_cast<Geometry::AABB&>(B);

        const Vec3  AC = a.GetCenter();

        const Vec3  MIN_B = b.GetMin();
        const Vec3  MAX_B = b.GetMax();

        // Find closest point on box B to centroid of sphere A
        Vec3 cP = AC;
        // X
        cP.x = std::max(cP.x, MIN_B.x);
        cP.x = std::min(cP.x, MAX_B.x);
        // Y
        cP.y = std::max(cP.y, MIN_B.y);
        cP.y = std::min(cP.y, MAX_B.y);
        // Z
        cP.z = std::max(cP.z, MIN_B.z);
        cP.z = std::min(cP.z, MAX_B.z);

        // Find squared distance from closest point to centroid of sphere A.
        // Check against sphere radius
        const float D_SQ  = Vec3::DistanceSquared(cP, AC);
        const bool RESULT = D_SQ <= (a.GetRadius() * a.GetRadius());
        if (RESULT)
        {
            A.SetIntersecting(true);
            B.SetIntersecting(true);
        }

        return RESULT;
    }

    bool CollisionController::AABBVSSphere(Geometry::Shape& A, Geometry::Shape& B)
    {
        return SphereVSAABB(B, A);
    }

    bool CollisionController::AABBVSAABB(Geometry::Shape& A, Geometry::Shape& B)
    {
        auto& a = reinterpret_cast<Geometry::AABB&>(A);
        auto& b = reinterpret_cast<Geometry::AABB&>(B);

        const Vec3 MIN_A = a.GetMin();  const Vec3 MIN_B = b.GetMin();
        const Vec3 MAX_A = a.GetMax();  const Vec3 MAX_B = b.GetMax();

        if (MAX_A.x < MIN_B.x || MIN_A.x > MAX_B.x) return false;
        if (MAX_A.y < MIN_B.y || MIN_A.y > MAX_B.y) return false;
        if (MAX_A.z < MIN_B.z || MIN_A.z > MAX_B.z) return false;

        A.SetIntersecting(true);
        B.SetIntersecting(true);
        return true;
    }

    bool CollisionController::SphereVSPlane(Geometry::Shape& A, Geometry::Shape& B)
    {
        auto& a = reinterpret_cast<Geometry::Sphere&>(A);
        auto& b = reinterpret_cast<Geometry::Plane&>(B);

        const Vec3& CENTROID_A  = a.GetCenter();
        const Vec3  NORMAL_B    = b.GetNormal();    // Guaranteed to be Normalised

        const float DIST = CENTROID_A.Dot(NORMAL_B) - b.GetDistance();
        const bool RESULT = std::abs(DIST) <= a.GetRadius();
        if (RESULT)
        {
            A.SetIntersecting(true);
            B.SetIntersecting(true);
        }

        return RESULT;
    }

    bool CollisionController::PlaneVSSphere(Geometry::Shape& A, Geometry::Shape& B)
    {
        return SphereVSPlane(B, A);
    }

    bool CollisionController::AABBVSPlane(Geometry::Shape& A, Geometry::Shape& B)
    {
        auto& a = reinterpret_cast<Geometry::AABB&>(A);
        auto& b = reinterpret_cast<Geometry::Plane&>(B);

        // Abs the half extents of the box
        const Vec3 EXTENTS_A = a.GetHalfExtents();

        // Abs the plane normal components
        Vec3 planeNormal = b.GetNormal();
        planeNormal.x = std::abs(planeNormal.x);
        planeNormal.y = std::abs(planeNormal.y);
        planeNormal.z = std::abs(planeNormal.z);

        // Projection Interval radius
        const float RADIUS = EXTENTS_A.Dot(planeNormal);
        // Distance from box to center of plane
        const float DISTANCE = planeNormal.Dot(a.GetCenter()) - b.GetDistance();

        const bool RESULT = std::abs(DISTANCE) <= RADIUS;
        if (RESULT)
        {
            A.SetIntersecting(true);
            B.SetIntersecting(true);
        }

        return RESULT;
    }

    bool CollisionController::PlaneVSAABB(Geometry::Shape& A, Geometry::Shape& B)
    {
        return AABBVSPlane(B, A);
    }




}
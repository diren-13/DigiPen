/************************************************************************************//*!
\file           AABB.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the implementation for the Axis-Aligned Bounding Box object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Geometry/AABB.h"
// Project Headers
#include "Math/CCMath.h"
#include "Tools/Console.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    AABB::AABB()
    : min   { Vec3::Zero }
    , max   { Vec3::One }
    {
        type = Type::AABB;
    }

    AABB::AABB(const AABB& rhs)
    : min   { rhs.min }
    , max   { rhs.max }
    {
        type = Type::AABB;

        supportPoints.clear();
        for (const auto& sp : rhs.supportPoints)
        {
            supportPoints.emplace_back(sp);
        }
    }

    AABB& AABB::operator=(const AABB& rhs)
    {
        type = Type::AABB;

        min = rhs.min;
        max = rhs.max;

        supportPoints.clear();
        for (const auto& sp : rhs.supportPoints)
        {
            supportPoints.emplace_back(sp);
        }

        return *this;
    }

    AABB::AABB(AABB&& rhs) noexcept
    : min   { rhs.min }
    , max   { rhs.max }
    {
        type = Type::AABB;

        supportPoints.clear();
        for (const auto& sp : rhs.supportPoints)
        {
            supportPoints.emplace_back(sp);
        }
    }

    AABB& AABB::operator=(AABB&& rhs) noexcept
    {
        type = Type::AABB;

        min = rhs.min;
        max = rhs.max;

        supportPoints.clear();
        for (const auto& sp : rhs.supportPoints)
        {
            supportPoints.emplace_back(sp);
        }

        return *this;
    }


    AABB::AABB(const Vec3& _min, const Vec3& _max)
    : min   { _min }
    , max   { _max }
    {
        // Check if min and max are swapped
        min.x = std::min(_min.x, _max.x);   max.x = std::max(_min.x, _max.x);
        min.y = std::min(_min.y, _max.y);   max.y = std::max(_min.y, _max.y);
        min.z = std::min(_min.z, _max.z);   max.z = std::max(_min.z, _max.z);

        type = Type::AABB;
    }

    AABB::AABB(const std::vector<Vec3>& vertices)
    : min   { Vec3::Zero }
    , max   { Vec3::Zero }
    {
        type = Type::AABB;
        computeTightFitBoundingBox(vertices.data(), static_cast<int>(vertices.size()));
    }

    AABB::AABB(const Vec3* vertices, int numVertices)
    : min   { Vec3::Zero }
    , max   { Vec3::Zero }
    {
        type = Type::AABB;
        computeTightFitBoundingBox(vertices, numVertices);
    }

    AABB::AABB(const std::vector<AABB>& aabbs)
    : min   { Vec3::Zero }
    , max   { Vec3::Zero }
    {
        type = Type::AABB;
        computeCombinedAABB(aabbs.data(), static_cast<unsigned int>(aabbs.size()));
    }

    AABB::AABB(const AABB* aabbs, unsigned numAABBs)
    : min   { Vec3::Zero }
    , max   { Vec3::Zero }
    {
        type = Type::AABB;
        computeCombinedAABB(aabbs, numAABBs);
    }



    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    const Vec3& AABB::GetMin() const noexcept
    {
        return min;
    }

    const Vec3& AABB::GetMax() const noexcept
    {
        return max;
    }
    
    Vec3 AABB::GetCenter() const noexcept
    {
        return Vec3::Lerp(min, max, 0.5f);
    }

    Vec3 AABB::GetHalfExtents() const noexcept
    {
        Vec3 halfExtents{Vec3::Zero};
        halfExtents.x = std::abs(max.x - min.x) * 0.5f;
        halfExtents.y = std::abs(max.y - min.y) * 0.5f;
        halfExtents.z = std::abs(max.z - min.z) * 0.5f;
        return halfExtents;
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void AABB::SetMin(const Vec3& _min) noexcept
    {
        min = _min;
    }

    void AABB::SetMax(const Vec3& _max) noexcept
    {
        max = _max;
    }

    void AABB::SetCenter(const Vec3& center) noexcept
    {
        const Vec3 HALF_EXTENTS = GetHalfExtents();
        min = center - HALF_EXTENTS;
        max = center + HALF_EXTENTS;
    }

    void AABB::SetHalfExtents(const Vec3& halfExtents) noexcept
    {
        const Vec3 CENTER = GetCenter();
        min = CENTER - halfExtents;
        max = CENTER + halfExtents;
    }

    void AABB::SetSupportPoints(const std::vector<Vec3>& vertices) noexcept
    {
        supportPoints.emplace_back(GetSupportPoint(vertices, -Vec3::UnitX), GetSupportPoint(vertices, Vec3::UnitX));
        supportPoints.emplace_back(GetSupportPoint(vertices, -Vec3::UnitY), GetSupportPoint(vertices, Vec3::UnitY));
        supportPoints.emplace_back(GetSupportPoint(vertices, -Vec3::UnitZ), GetSupportPoint(vertices, Vec3::UnitZ));
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void AABB::Update(const Transform& tf)
    {
        intersecting = false;

        Vec3 newMin{ std::numeric_limits<float>::max() };
        Vec3 newMax{ std::numeric_limits<float>::lowest() };

        for (const auto& sp : supportPoints)
        {
            const Vec3 SP_MIN = Vec3::Transform(sp.first, tf.GetTRS());
            const Vec3 SP_MAX = Vec3::Transform(sp.second, tf.GetTRS());

            newMin.x = std::min({ newMin.x, SP_MIN.x, SP_MAX.x });
            newMin.y = std::min({ newMin.y, SP_MIN.y, SP_MAX.y });
            newMin.z = std::min({ newMin.z, SP_MIN.z, SP_MAX.z });

            newMax.x = std::max({ newMax.x, SP_MIN.x, SP_MAX.x });
            newMax.y = std::max({ newMax.y, SP_MIN.y, SP_MAX.y });
            newMax.z = std::max({ newMax.z, SP_MIN.z, SP_MAX.z });
        }

        min = newMin;
        max = newMax;
    }


    bool AABB::TestPoint(const Vec3& point, float)
    {
        if (point.x < min.x || point.x > max.x /*|| std::fabs(point.x - min.x) <= Math::EPSILON || std::fabs(point.x - max.x) <= Math::EPSILON*/)
            return false;
        if (point.y < min.y || point.y > max.y /*|| std::fabs(point.y - min.y) <= Math::EPSILON || std::fabs(point.y - max.y) <= Math::EPSILON*/)
            return false;
        if (point.z < min.z || point.z > max.z /*|| std::fabs(point.z - min.z) <= Math::EPSILON || std::fabs(point.z - max.z) <= Math::EPSILON*/) 
            return false;

        intersecting = true;
        return true;
    }

    bool AABB::Raycast(Ray& ray)
    {
        float tMin = 0.0f;
        float tMax = std::numeric_limits<float>::max();

        const Vec3& P = ray.GetPosition();
        const Vec3& D = ray.GetDirection();

        // Check if ray is parallel to slab
        if (std::abs(D.x) < Math::EPSILON && (P.x < min.x || P.x > max.x)) return false;
        if (std::abs(D.y) < Math::EPSILON && (P.y < min.y || P.y > max.y)) return false;
        if (std::abs(D.z) < Math::EPSILON && (P.z < min.z || P.z > max.z)) return false;

        const Vec3 INV_D{ 1.0f / D.x, 1.0f / D.y, 1.0f / D.z };

        // X
        float t1x = (min.x - P.x) * INV_D.x;
        float t2x = (max.x - P.x) * INV_D.x;
        if (t1x > t2x)
        {
            std::swap(t1x, t2x);
        }

        tMin = std::max(tMin, t1x);
        tMax = std::min(tMax, t2x);

        if (tMin > tMax)
            return false;

        // Y
        float t1y = (min.y - P.y) * INV_D.y;
        float t2y = (max.y - P.y) * INV_D.y;
        if (t1y > t2y)
        {
            std::swap(t1y, t2y);
        }

        tMin = std::max(tMin, t1y);
        tMax = std::min(tMax, t2y);

        if (tMin > tMax)
            return false;

        // Z
        float t1z = (min.z - P.z) * INV_D.z;
        float t2z = (max.z - P.z) * INV_D.z;
        if (t1z > t2z)
        {
            std::swap(t1z, t2z);
        }

        tMin = std::max(tMin, t1z);
        tMax = std::min(tMax, t2z);

        if (tMin > tMax)
            return false;

        ray.t = tMin;

        intersecting = true;
        return true;
    }

    bool AABB::Contains(const AABB& rhs) const
    {
        if (rhs.min.x < min.x || rhs.max.x > max.x) return false;
        if (rhs.min.y < min.y || rhs.max.y > max.y) return false;
        if (rhs.min.z < min.z || rhs.max.z > max.z) return false;

        return true;
    }


    void AABB::Combine(const AABB& rhs)
    {
        if (Contains(rhs))
            return;

        min.x = std::min(min.x, rhs.min.x); max.x = std::max(max.x, rhs.max.x);
        min.y = std::min(min.y, rhs.min.y); max.y = std::max(max.y, rhs.max.y);
        min.z = std::min(min.z, rhs.min.z); max.z = std::max(max.z, rhs.max.z);
    }

    float AABB::Volume() const
    {
        const Vec3 HALF_EXTENTS = GetHalfExtents();
        return 8.0f * (HALF_EXTENTS.x * HALF_EXTENTS.y * HALF_EXTENTS.z);
    }

    float AABB::SurfaceArea() const
    {
        const Vec3 HALF_EXTENTS = GetHalfExtents();
        return 8.0f * ((HALF_EXTENTS.x * HALF_EXTENTS.y) + (HALF_EXTENTS.x * HALF_EXTENTS.z) + (HALF_EXTENTS.y * HALF_EXTENTS.z));
        
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Member Definitions                                             */
    /*---------------------------------------------------------------------------------*/
    void AABB::computeCombinedAABB(const AABB* aabbs, unsigned numAABBs)
    {
        if (numAABBs <= 0)
        {
            #ifdef _DEBUG
                Log(LogSeverity::Warning, "No bounding boxes passed in!");
            #endif
            return;
        }

        min = aabbs->min;
        max = aabbs->max;

        if (numAABBs == 1)
            return;

        const auto COMBINE_FUNCTION = std::bind(&AABB::Combine, this, std::placeholders::_1);
        std::for_each_n(aabbs + 1, numAABBs - 1, COMBINE_FUNCTION);
    }

    void AABB::computeTightFitBoundingBox(const Vec3* vertices, int numVertices)
    {
        min = Vec3{ std::numeric_limits<float>::max() };
        max = Vec3{ std::numeric_limits<float>::lowest() };

        for (int i = 0; i < numVertices; ++i)
        {
            const Vec3& V = vertices[i];

            min.x = std::min(min.x, V.x);
            min.y = std::min(min.y, V.y);
            min.z = std::min(min.z, V.z);

            max.x = std::max(max.x, V.x);
            max.y = std::max(max.y, V.y);
            max.z = std::max(max.z, V.z);
        }
    }
}
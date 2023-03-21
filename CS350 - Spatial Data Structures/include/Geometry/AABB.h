/************************************************************************************//*!
\file           AABB.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the interface for the Axis-Aligned Bounding Box object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
// Project Headers
#include "Shape.h"
#include "Math/Transform.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    class AABB final : public Shape
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        AABB            ();
        AABB            (const AABB& rhs);
        AABB& operator= (const AABB& rhs);
        AABB            (AABB&& rhs) noexcept;
        AABB& operator= (AABB&& rhs) noexcept;

        AABB            (const Vec3& min, const Vec3& max);
        AABB            (const std::vector<Vec3>& vertices);
        AABB            (const Vec3* vertices, int numVertices);
        AABB            (const std::vector<AABB>& aabbs);
        AABB            (const AABB* aabbs, unsigned int numAABBs);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const Vec3&   GetMin          ()  const noexcept;
        [[nodiscard]] const Vec3&   GetMax          ()  const noexcept;
        [[nodiscard]] Vec3          GetCenter       ()  const noexcept;
        [[nodiscard]] Vec3          GetHalfExtents  ()  const noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetMin             (const Vec3& min)                   noexcept;
        void SetMax             (const Vec3& max)                   noexcept;
        void SetCenter          (const Vec3& center)                noexcept;
        void SetHalfExtents     (const Vec3& halfExtents)           noexcept;
        void SetSupportPoints   (const std::vector<Vec3>& vertices) noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void                Update      (const Transform& tf)                           override;
        [[nodiscard]] bool  TestPoint   (const Vec3& point, float e = Math::EPSILON)    override;
        [[nodiscard]] bool  Raycast     (Ray& ray)                                      override;
        [[nodiscard]] bool  Contains    (const AABB& rhs)                               const;
        void                Combine     (const AABB& rhs);

        [[nodiscard]] float Volume      ()                                              const;
        [[nodiscard]] float SurfaceArea ()                                              const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using SupportPointPair = std::pair<Vec3, Vec3>;

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Vec3                            min;
        Vec3                            max;
        std::vector<SupportPointPair>   supportPoints;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void computeCombinedAABB        (const AABB* aabbs, unsigned int numAABBs);
        void computeTightFitBoundingBox (const Vec3* vertices, int numVertices);
    };
}

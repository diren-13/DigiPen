/************************************************************************************//*!
\file           Sphere.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the interface for the Sphere object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Shape.h"
#include "AABB.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    class Sphere final : public Shape
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        enum class Method
        {
            Ritter
        ,   Larsson_6
        ,   Larsson_14
        ,   Larsson_26
        ,   Larsson_98
        ,   PCA
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Sphere              ();
        Sphere              (const Sphere& rhs);
        Sphere& operator=   (const Sphere& rhs) = default;
        Sphere              (Sphere&& rhs)      noexcept;
        Sphere& operator=   (Sphere&& rhs)      = default;

        Sphere              (const Vec3& c, float r, const Transform& tf = Transform{});
        Sphere              (const std::vector<Vec3>& points, Method method = Method::Ritter);
        Sphere              (const Vec3* points, unsigned int numPoints, Method method = Method::Ritter);
        Sphere              (const AABB& aabb);
        Sphere              (const AABB& aabb, const std::vector<Vec3>& points);
        Sphere              (const AABB& aabb, const Vec3* points, unsigned int numPoints);
        Sphere              (const std::vector<Sphere>& spheres);
        Sphere              (const Sphere* spheres, unsigned int numSpheres);


        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] Method        GetMethod       ()  const noexcept;
        [[nodiscard]] const Vec3&   GetLocalCenter  ()  const noexcept;
        [[nodiscard]] const Vec3&   GetCenter       ()  const noexcept;
        [[nodiscard]] float         GetRadius       ()  const noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetLocalCenter  (const Vec3& lc)    noexcept;
        void    SetCenter       (const Vec3& c)     noexcept;
        void    SetRadius       (float r)           noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void                Update      (const Transform& tf)                           override;
        [[nodiscard]] bool  TestPoint   (const Vec3& point, float e = Math::EPSILON)    override;
        [[nodiscard]] bool  Raycast     (Ray& ray)                                      override;
        [[nodiscard]] bool  Contains    (const Sphere& rhs)                             const;
        void                Combine     (const Sphere& rhs);

        [[nodiscard]] float Volume      ()                                              const;
        [[nodiscard]] float SurfaceArea ()                                              const;

        void                Build       (const std::vector<Vec3>& points, Method method = Method::Ritter);
        void                Build       (const Vec3* points, unsigned int numPoints, Method method = Method::Ritter);
        void                Build       (const AABB& aabb);
        void                Build       (const AABB& aabb, const std::vector<Vec3>& points);
        void                Build       (const AABB& aabb, const Vec3* points, unsigned int numPoints);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Method  method;

        Vec3    localCenter;
        Vec3    center;
        float   radius;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        void computeCombinedSphere  (const Sphere* spheres, unsigned int numSpheres);

        // Ritter/Larsson (Ritter's is EPOS-6)
        void ritterSphere           (const Vec3* points, unsigned int numPoints);
        void larssonSphere          (const Vec3* points, unsigned int numPoints, Method _method);
        // PCA
        void pcaSphere              (const Vec3* points, unsigned int numPoints);
        Mat4 covarianceMatrix       (const Vec3* points, unsigned int numPoints);
        Mat4 computeJacobian        (const Mat4& J, const Mat4& covariance);
        Vec3 getEigenValueVector    (const Mat4& v, const Mat4& covariance);
        // Post-Estimated Sphere
        void growSphere             (const std::vector<Vec3>& points);
    };
}

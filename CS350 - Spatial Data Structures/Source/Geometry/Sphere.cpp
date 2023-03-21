/************************************************************************************//*!
\file           Sphere.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the implementation for the Sphere object.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Geometry/Sphere.h"
// Project Headers
#include "Tools/Console.h"

const std::vector EPOS_6 =
{
    ClamChowder::Vec3{ 1.0f, 0.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 0.0f, 1.0f }
};

const std::vector EPOS_14 =
{
    ClamChowder::Vec3{ 1.0f, 0.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 0.0f, 1.0f },
    ClamChowder::Vec3{ 1.0f, 1.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 1.0f,-1.0f }, ClamChowder::Vec3{ 1.0f,-1.0f, 1.0f }, ClamChowder::Vec3{ 1.0f,-1.0f,-1.0f }
};

const std::vector EPOS_26 =
{
    ClamChowder::Vec3{ 1.0f, 0.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 0.0f, 1.0f },
    ClamChowder::Vec3{ 1.0f, 1.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 1.0f,-1.0f }, ClamChowder::Vec3{ 1.0f,-1.0f, 1.0f }, ClamChowder::Vec3{ 1.0f,-1.0f,-1.0f },
    ClamChowder::Vec3{ 1.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 1.0f,-1.0f, 0.0f }, ClamChowder::Vec3{ 1.0f, 0.0f, 1.0f },
    ClamChowder::Vec3{ 1.0f, 0.0f,-1.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 1.0f }, ClamChowder::Vec3{ 0.0f, 1.0f,-1.0f }
};

const std::vector EPOS_98 =
{
    ClamChowder::Vec3{ 1.0f, 0.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 0.0f, 0.0f, 1.0f },
    ClamChowder::Vec3{ 1.0f, 1.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 1.0f,-1.0f }, ClamChowder::Vec3{ 1.0f,-1.0f, 1.0f }, ClamChowder::Vec3{ 1.0f,-1.0f,-1.0f },
    ClamChowder::Vec3{ 1.0f, 1.0f, 0.0f }, ClamChowder::Vec3{ 1.0f,-1.0f, 0.0f }, ClamChowder::Vec3{ 1.0f, 0.0f, 1.0f },
    ClamChowder::Vec3{ 1.0f, 0.0f,-1.0f }, ClamChowder::Vec3{ 0.0f, 1.0f, 1.0f }, ClamChowder::Vec3{ 0.0f, 1.0f,-1.0f },
    ClamChowder::Vec3{ 0.0f, 1.0f, 2.0f }, ClamChowder::Vec3{ 0.0f, 2.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 0.0f, 2.0f }, ClamChowder::Vec3{ 2.0f, 0.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 2.0f, 0.0f }, ClamChowder::Vec3{ 2.0f, 1.0f, 0.0f },
    ClamChowder::Vec3{ 0.0f, 1.0f,-2.0f }, ClamChowder::Vec3{ 0.0f, 2.0f,-1.0f }, ClamChowder::Vec3{ 1.0f, 0.0f,-2.0f }, ClamChowder::Vec3{ 2.0f, 0.0f,-1.0f }, ClamChowder::Vec3{ 1.0f,-2.0f, 0.0f }, ClamChowder::Vec3{ 2.0f,-1.0f, 0.0f },
    ClamChowder::Vec3{ 1.0f, 1.0f, 2.0f }, ClamChowder::Vec3{ 2.0f, 1.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 2.0f, 1.0f }, ClamChowder::Vec3{ 1.0f,-1.0f, 2.0f }, ClamChowder::Vec3{ 1.0f, 1.0f,-2.0f }, ClamChowder::Vec3{ 1.0f,-1.0f,-2.0f },
    ClamChowder::Vec3{ 2.0f,-1.0f, 1.0f }, ClamChowder::Vec3{ 2.0f, 1.0f,-1.0f }, ClamChowder::Vec3{ 2.0f,-1.0f,-1.0f }, ClamChowder::Vec3{ 1.0f,-2.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 2.0f,-1.0f }, ClamChowder::Vec3{ 1.0f,-2.0f,-1.0f },
    ClamChowder::Vec3{ 2.0f, 2.0f, 1.0f }, ClamChowder::Vec3{ 1.0f, 2.0f, 2.0f }, ClamChowder::Vec3{ 2.0f, 1.0f, 2.0f }, ClamChowder::Vec3{ 2.0f,-2.0f, 1.0f }, ClamChowder::Vec3{ 2.0f, 2.0f,-1.0f }, ClamChowder::Vec3{ 2.0f,-2.0f,-1.0f },
    ClamChowder::Vec3{ 1.0f,-2.0f, 2.0f }, ClamChowder::Vec3{ 1.0f, 2.0f,-2.0f }, ClamChowder::Vec3{ 1.0f,-2.0f,-2.0f }, ClamChowder::Vec3{ 2.0f,-1.0f, 2.0f }, ClamChowder::Vec3{ 2.0f, 1.0f,-2.0f }, ClamChowder::Vec3{ 2.0f,-1.0f,-2.0f }
};

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    Sphere::Sphere()
    : method    { Method::Ritter }
    , center    { Vec3::Zero }
    , radius    { 1.0f }
    {
        type = Type::Sphere;
    }

    Sphere::Sphere(const Sphere& rhs)
    : method        { rhs.method }
    , localCenter   { rhs.localCenter }
    , center        { rhs.center }
    , radius        { rhs.radius }
    {
        type = Type::Sphere;
    }

    Sphere::Sphere(Sphere&& rhs) noexcept
    : method        { rhs.method }
    , localCenter   { rhs.localCenter }
    , center        { rhs.center }
    , radius        { rhs.radius }
    {
        type = Type::Sphere; 
    }

    Sphere::Sphere(const Vec3& c, float r, const Transform& tf)
    : method        { Method::Ritter }
    , localCenter   { Vec3::Transform(c, tf.GetTRS().Invert()) }
    , center        { c }
    , radius        { std::abs(r) }
    {
        type = Type::Sphere;
    }

    Sphere::Sphere(const std::vector<Vec3>& points, Method _method)
    : method    { _method }
    , radius    { 0.0f }
    {
        type = Type::Sphere;
        Build(points, method);
    }

    Sphere::Sphere(const Vec3* points, unsigned numPoints, Method _method)
    : method    { _method }
    , radius    { 0.0f }
    {
        type = Type::Sphere;
        Build(points, numPoints, method);
    }

    Sphere::Sphere(const AABB& aabb)
    : method    { Method::Ritter }
    , radius    { 0.0f }
    {
        type = Type::Sphere;
        Build(aabb);
    }

    Sphere::Sphere(const AABB& aabb, const std::vector<Vec3>& points)
    : method    { Method::Ritter }
    , radius    { 0.0f }
    {
        type = Type::Sphere;
        Build(aabb, points);
    }

    Sphere::Sphere(const AABB& aabb, const Vec3* points, unsigned numPoints)
    : method    { Method::Ritter }
    , radius    { 0.0f }
    {
        type = Type::Sphere;
        Build(aabb, points, numPoints);
    }

    Sphere::Sphere(const std::vector<Sphere>& spheres)
    : method        { spheres.front().method }
    , localCenter   { spheres.front().localCenter }
    , center        { spheres.front().center }
    , radius        { spheres.front().radius }
    {
        type = Type::Sphere;
        computeCombinedSphere(spheres.data(), static_cast<unsigned int>(spheres.size()));
    }

    Sphere::Sphere(const Sphere* spheres, unsigned numSpheres)
    : method        { spheres->method }
    , localCenter   { spheres->localCenter }
    , center        { spheres->center }
    , radius        { spheres->radius }
    {
        type = Type::Sphere;
        computeCombinedSphere(spheres, numSpheres);
    }


    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    Sphere::Method Sphere::GetMethod() const noexcept
    {
        return method;
    }

    const Vec3& Sphere::GetLocalCenter() const noexcept
    {
        return localCenter;
    }

    const Vec3& Sphere::GetCenter() const noexcept
    {
        return center;
    }

    float Sphere::GetRadius() const noexcept
    {
        return radius;
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Sphere::SetLocalCenter(const Vec3& lc) noexcept
    {
        localCenter = lc;
    }

    void Sphere::SetCenter(const Vec3& c) noexcept
    {
        center = c;
    }

    void Sphere::SetRadius(float r) noexcept
    {
        radius = r;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void Sphere::Update(const Transform& tf)
    {
        intersecting = false;
        center = Vec3::Transform(localCenter, tf.GetTRS());
    }

    bool Sphere::TestPoint(const Vec3& point, float)
    {
        const float D = Vec3::DistanceSquared(center, point);
        const bool TEST = D <= (radius * radius);
        if (TEST)
        {
            intersecting = true;
        }
        return TEST;
    }

    bool Sphere::Raycast(Ray& ray)
    {
        const Vec3 M = ray.GetPosition() - center;

        // Solve quadratic equation
        const float B = M.Dot(ray.GetDirection());
        const float C = M.LengthSquared() - (radius * radius);

        if (B > 0.0f && C > 0.0f)
            return false;

        const float DISCRIMINANT = (B * B) - C;
        if (DISCRIMINANT < 0.0f)
            return false;
        
        ray.t = -B - std::sqrtf(DISCRIMINANT);
        ray.t = std::clamp(0.0f, ray.t, std::numeric_limits<float>::max());

        intersecting = true;
        return true;
    }

    bool Sphere::Contains(const Sphere& rhs) const
    {
        const float RAD_SQ = radius * radius;

        // Distance to extents
        const float VEC_DIST_SQ = Vec3::DistanceSquared(center, rhs.center);
        const float RHS_RAD_SQ  = rhs.radius * rhs.radius;

        return RAD_SQ >= (VEC_DIST_SQ + RHS_RAD_SQ) || Math::CompareFloat(RAD_SQ - (VEC_DIST_SQ + RHS_RAD_SQ), 0.0f);
    }

    void Sphere::Combine(const Sphere& rhs)
    {
        if (Contains(rhs))
            return;

        const float R0  = radius;
        const float R1  = rhs.radius;
        const Vec3& C0  = center;
        const Vec3& C1  = rhs.center;

        const float D   = Vec3::Distance(C0, C1);

        radius = (D + R0 + R1) * 0.5f;
        center = C0;
        if (D > Math::EPSILON)
        {
            center += ((radius - R0) / D) * (C1 - C0);
        }
    }

    float Sphere::Volume() const
    {
        static constexpr float FOUR_THIRDS_TIMES_PI = (4.0f / 3.0f) * DirectX::XM_PI;
        return FOUR_THIRDS_TIMES_PI * (radius * radius * radius);
    }

    float Sphere::SurfaceArea() const
    {
        static constexpr float FOUR_TIMES_PI = 4.0f * DirectX::XM_PI;
        return FOUR_TIMES_PI * (radius * radius);
    }

    void Sphere::Build(const std::vector<Vec3>& points, Method _method)
    {
        method = _method;
        Build(points.data(), static_cast<unsigned>(points.size()), method);
    }

    void Sphere::Build(const Vec3* points, unsigned numPoints, Method _method)
    {
        method = _method;
        switch (method)
        {
            case Method::Ritter:        ritterSphere    (points, numPoints);                     break;
            case Method::Larsson_6:     larssonSphere   (points, numPoints, Method::Larsson_6);  break;
            case Method::Larsson_14:    larssonSphere   (points, numPoints, Method::Larsson_14); break;
            case Method::Larsson_26:    larssonSphere   (points, numPoints, Method::Larsson_26); break;
            case Method::Larsson_98:    larssonSphere   (points, numPoints, Method::Larsson_98); break;
            case Method::PCA:           pcaSphere       (points, numPoints);                     break;
        }

        std::vector remainingPoints{ numPoints, Vec3::Zero };
        std::copy_n(points, numPoints, remainingPoints.begin());
        growSphere(remainingPoints);
    }

    void Sphere::Build(const AABB& aabb)
    {
        center = aabb.GetCenter();
        radius = (aabb.GetMax() - aabb.GetMin()).Length() * 0.5f;

        // Compute local center
        Transform tmp{ aabb.GetCenter(), Vec3::Zero, aabb.GetHalfExtents() * 2.0f };
        localCenter = Vec3::Transform(center, tmp.GetTRS().Invert());
    }

    void Sphere::Build(const AABB& aabb, const std::vector<Vec3>& points)
    {
        Build(aabb, points.data(), static_cast<unsigned>(points.size()));
    }


    void Sphere::Build(const AABB& aabb, const Vec3* points, unsigned numPoints)
    {
        center = aabb.GetCenter();

        float dist = std::numeric_limits<float>::lowest();
        for (unsigned i = 0; i < numPoints; ++i)
        {
            const float D = (points[i] - center).LengthSquared();
            dist = std::max(dist, D);
        }

        radius = std::sqrtf(dist);

        // Compute local center
        localCenter = Vec3::Transform(center, Transform{ aabb.GetCenter(), Vec3::Zero, aabb.GetHalfExtents() * 2.0f }.GetTRS().Invert());
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Member Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void Sphere::computeCombinedSphere(const Sphere* spheres, unsigned numSpheres)
    {
        if (numSpheres <= 0)
        {
            Log(LogSeverity::Error, "Invalid number of bounding volumes passed in!");
            return;
        }

        const auto COMBINE_FUNCTION = std::bind(&Sphere::Combine, this, std::placeholders::_1);
        std::for_each_n(spheres, numSpheres, COMBINE_FUNCTION);
    }


    void Sphere::ritterSphere(const Vec3* points, unsigned int numPoints)
    {
        larssonSphere(points, numPoints, Method::Larsson_6);
        method = Method::Ritter;
    }

    void Sphere::larssonSphere(const Vec3* points, unsigned numPoints, Method _method)
    {
        const std::vector<Vec3>* epos = nullptr;

        // Find EPOS
        method = _method;
        switch (method)
        {
            case Method::Larsson_6:     epos = &EPOS_6;     break;
            case Method::Larsson_14:    epos = &EPOS_14;    break;
            case Method::Larsson_26:    epos = &EPOS_26;    break;
            case Method::Larsson_98:    epos = &EPOS_98;    break;
            default:                    return;
        }

        float largestDistance = std::numeric_limits<float>::lowest();
        for (const auto& n : *epos)
        {
            const Vec3 MIN = GetSupportPoint(points, numPoints, -n);
            const Vec3 MAX = GetSupportPoint(points, numPoints, n);

            const float D = (MAX - MIN).LengthSquared();
            if (D > largestDistance)
            {
                largestDistance = D;
                center = Vec3::Lerp(MIN, MAX, 0.5f);
            }
        }

        radius = std::sqrtf(largestDistance) * 0.5f;
    }

    void Sphere::pcaSphere(const Vec3* points, unsigned numPoints)
    {
        static constexpr int MAX_ITERATIONS = 50;

        Mat4 A = covarianceMatrix(points, numPoints);

        float prevOffset = 0.0f;

        Mat4 v{ Mat4::Identity };
        Mat4 J{ Mat4::Identity };

        // Find largest off-diagonal absolute element A[p][q]
        for (int n = 0; n < MAX_ITERATIONS; ++n)
        {
            J = computeJacobian(J, A);

            // Cumulate rotations into what will contain the eigenvectors
            v = v * J;
            // Make ’A’ more diagonal, until just eigenvalues remain on diagonal
            A = (J.Transpose() * A) * J;

            // Compute "norm" of off-diagonal elements
            float offset = 0.0f;

            for (int i = 0; i < 3; ++i)
            {
                for (int j = i + 1; j < 3; ++j)
                {
                    offset += A.m[i][j] * A.m[i][j];
                }
            }

            if (n > 2 && offset >= prevOffset)
                break;

            prevOffset = offset;
        }

        const Vec3 E = getEigenValueVector(v, A);

        // Get initial support points along E
        Vec3 min = GetSupportPoint(points, numPoints, -E);
        Vec3 max = GetSupportPoint(points, numPoints, E);

        float supportPointDist = Vec3::DistanceSquared(min, max);

        float maxDistance = std::numeric_limits<float>::lowest();
        float minDistance = std::numeric_limits<float>::max();
        for (unsigned int i = 0; i < numPoints; ++i)
        {
            const Vec3& CURRENT_VERTEX = points[i];
            const float MAX_PROJECTION = E.Dot(CURRENT_VERTEX);
            const float MIN_PROJECTION = -E.Dot(CURRENT_VERTEX);

            if (MAX_PROJECTION > maxDistance)
            {
                maxDistance = MAX_PROJECTION;
                if (const float MAX_TO_MIN_DIST = Vec3::DistanceSquared(CURRENT_VERTEX, min); MAX_TO_MIN_DIST > supportPointDist)
                {
                    supportPointDist = MAX_TO_MIN_DIST;
                    min = CURRENT_VERTEX;
                }
            }

            if (MIN_PROJECTION < minDistance)
            {
                minDistance = MIN_PROJECTION;
                if (const float MIN_TO_MAX_DIST = Vec3::DistanceSquared(CURRENT_VERTEX, max); MIN_TO_MAX_DIST > supportPointDist)
                {
                    supportPointDist = MIN_TO_MAX_DIST;
                    max = CURRENT_VERTEX;
                }
            }
        }

        center = Vec3::Lerp(min, max, 0.5f);
        radius = Vec3::Distance(min, max) * 0.5f;
    }

    Mat4 Sphere::covarianceMatrix(const Vec3* points, unsigned numPoints)
    {
        const float ONE_OVER_N = 1.0f / static_cast<float>(numPoints);

        center = Vec3::Zero;
        // Compute centroid
        center = std::accumulate(points, points + numPoints, Vec3::Zero);
        center *= ONE_OVER_N;

        Mat4 covarianceMtx{ Vec3::Zero, Vec3::Zero, Vec3::Zero };

        static auto compute = [&](const Vec3& point)->void
        {
            const Vec3 P = point - center;

            covarianceMtx.m[0][0] += (P.x * P.x);
            covarianceMtx.m[1][1] += (P.y * P.y);
            covarianceMtx.m[2][2] += (P.z * P.z);
            covarianceMtx.m[0][1] += (P.x * P.y);
            covarianceMtx.m[0][2] += (P.x * P.z);
            covarianceMtx.m[1][2] += (P.y * P.z);
        };

        std::for_each_n(points, numPoints, compute);

        covarianceMtx.m[0][0] *= ONE_OVER_N;
        covarianceMtx.m[1][1] *= ONE_OVER_N;
        covarianceMtx.m[2][2] *= ONE_OVER_N;
        covarianceMtx.m[0][1] *= ONE_OVER_N;
        covarianceMtx.m[0][2] *= ONE_OVER_N;
        covarianceMtx.m[1][2] *= ONE_OVER_N;
        covarianceMtx.m[1][0] = covarianceMtx.m[0][1];
        covarianceMtx.m[2][0] = covarianceMtx.m[0][2];
        covarianceMtx.m[2][1] = covarianceMtx.m[1][2];

        return covarianceMtx;
    }

    Mat4 Sphere::computeJacobian(const Mat4& J, const Mat4& covariance)
    {
        Mat4 outJ = J;

        int p = 0;
        int q = 1;
        for (int j = 0; j < 3; ++j)
        {
            for (int k = j + 1; k < 3; ++k)
            {
                if (std::abs(covariance.m[j][k]) > std::abs(covariance.m[p][q]))
                {
                    p = j;
                    q = k;
                }
            }
        }

        // Compute the Jacobi rotation matrix J(p, q, theta)
        float sin = 0.0f;
        float cos = 0.0f;
        if (std::abs(covariance.m[p][q]) > Math::EPSILON)
        {
            float tan = 0.0f;

            if (const float R = (covariance.m[q][q] - covariance.m[p][p]) / (2.0f * covariance.m[p][q]); R >= 0.0f)
            {
                tan = 1.0f / (R + std::sqrtf(1.0f + (R * R)));
            }
            else
            {
                tan = -1.0f / (-R + std::sqrtf(1.0f + (R * R)));
            }

            cos = 1.0f / std::sqrt(1.0f + (tan * tan));
            sin = tan * cos;
        }
        else
        {
            cos = 1.0f;
            sin = 0.0f;
        }


        for (int i = 0; i < 3; ++i)
        {
            outJ.m[i][0] = outJ.m[i][1] = outJ.m[i][2] = 0.0f;
            outJ.m[i][i] = 1.0f;
        }

        outJ.m[p][p] = cos;  outJ.m[p][q] = sin;
        outJ.m[q][p] = -sin; outJ.m[q][q] = cos;

        return outJ;
    }

    Vec3 Sphere::getEigenValueVector(const Mat4& v, const Mat4& covariance)
    {
        // Find the component with largest magnitude eigenvalue (largest spread)
        Vec3 e;
        int maxC = 0;
        float maxF, maxE = std::abs(covariance.m[0][0]);
        if ((maxF = std::abs(covariance.m[1][1])) > maxE)
        {
            maxC = 1;
            maxE = maxF;
        }
        if ((maxF = std::abs(covariance.m[2][2])) > maxE)
        {
            maxC = 2;
            maxE = maxF;
        }

        e.x = v.m[0][maxC];
        e.y = v.m[1][maxC];
        e.z = v.m[2][maxC];

        return e;
    }


    void Sphere::growSphere(const std::vector<Vec3>& points)
    {
        for (const auto& p : points)
        {
            if (TestPoint(p))
                continue;

            // Point is outside, Grow sphere to encompass point
            Vec3 v = (p - center);
            v.Normalize();

            const Vec3 Q = center - (v * radius);

            center = Vec3::Lerp(p, Q, 0.5f);
            radius = (p - Q).Length() * 0.5f;
        }
    }
}
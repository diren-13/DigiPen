/************************************************************************************//*!
\file           Shape.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the implementation for the Shape object, which is the base object
                used to represent bounding volumes.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Geometry/Shape.h"

namespace ClamChowder::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    Vec3 Shape::GetSupportPoint(const std::vector<Vec3>& vertices, const Vec3& direction)
    {
        return GetSupportPoint(vertices.data(), static_cast<int>(vertices.size()), direction);
    }

    Vec3 Shape::GetSupportPoint(const Vec3* vertices, int numVertices, const Vec3& direction)
    {
        float largestDistance = std::numeric_limits<float>::lowest();
        Vec3 supportPoint;

        for (int i = 0; i < numVertices; ++i)
        {
            const Vec3& CURRENT_VERTEX = vertices[i];
            const float PROJECTION = direction.Dot(CURRENT_VERTEX);

            if (PROJECTION > largestDistance)
            {
                supportPoint = CURRENT_VERTEX;
                largestDistance = PROJECTION;
            }
        }

        return supportPoint;
    }

}
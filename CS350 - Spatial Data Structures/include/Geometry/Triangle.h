/************************************************************************************//*!
\file           Triangle.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 14, 2022
\brief          Contains the interface for the Triangle object.
 
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

    /********************************************************************************//*!
    @brief    Encapsulates a 2D Triangle.
    *//*********************************************************************************/
    class Triangle final : public Shape
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr size_t NUM_POINTS = 3;


        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Triangle            ();
        Triangle            (const Triangle& rhs);
        Triangle& operator= (const Triangle& rhs)   = default;
        Triangle            (Triangle&& rhs) noexcept;
        Triangle& operator= (Triangle&& rhs)        = default;

        Triangle            (const Vec3& vertex0, const Vec3& vertex1, const Vec3& vertex2);
        //Triangle            (const std::vector<Vec3>& vertices);
        //Triangle            (const Vec3* vertices, int numVertices);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const Vec3&   GetVertex0  ()  const   { return v0; }
        [[nodiscard]] const Vec3&   GetVertex1  ()  const   { return v1; }
        [[nodiscard]] const Vec3&   GetVertex2  ()  const   { return v2; }

        [[nodiscard]] Vec3          GetEdge1    ()  const;
        [[nodiscard]] Vec3          GetEdge2    ()  const;
        [[nodiscard]] Vec3          GetEdge3    ()  const;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] void Update       (const Transform& tf)                           override;
        [[nodiscard]] bool TestPoint    (const Vec3& point, float e = Math::EPSILON)    override;
        [[nodiscard]] bool Raycast      (Ray& ray)                                      override;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Vec3    v0;
        Vec3    v1;
        Vec3    v2;
        float   signedArea2;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        float inv2SignedArea() const; 


    };
}

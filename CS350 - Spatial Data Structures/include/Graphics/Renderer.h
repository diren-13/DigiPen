/************************************************************************************//*!
\file           Renderer.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2022
\brief          Encapsulation of the Rendering system of the framework.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
// DirectX Headers
#include <PrimitiveBatch.h>
// Project Headers
#include "Device.h"
#include "Scene.h"
#include "Light.h"
#include "Geometry/AABBTree.h"
#include "Geometry/BSphereTree.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Renderer
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        enum class DebugDraw
        {
            AABB
        ,   RitterSphere
        ,   LarssonSphere
        ,   PCASphere
        ,   AABBTree
        ,   RitterSphereTree
        ,   LarssonSphereTree
        ,   PCASphereTree
        ,   Octree
        ,   OctreeData      // Very Specific
        ,   BSPTreeData     // Very Specific

        ,   TOTAL
        };

        struct alignas(16) DefaultVSCBuffer
        {
            Colour  Colour;
            Mat4    World;
            Mat4    ViewProjection;
        };

        struct alignas(16) DefaultPSCBuffer
        {
            DirectionalLight    DirectionalLights[Light::MAX_LIGHTS];
            unsigned int        DirectionalLightCount;
            Vec3                CameraPos;
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Renderer(Device& device);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const Colour& GetClearColour      ()                  const   { return clearColour; }
        [[nodiscard]] bool          GetDebugDrawState   (DebugDraw flag)   const;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetClearColour     (const Colour& colour)          { clearColour = colour; }
        void SetDebugDrawState  (DebugDraw flag, bool state);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        void RenderAxes     (const View& defaultView) const;
        void RenderGrid     (const View& defaultView) const;

        void Draw           (Device& device, const Scene& scene, const View& view) const;
        void DrawLine       (Device& device, const View& view, const Vec3& p0, const Vec3& p1, const Colour& clr = Colour{ Colours::White }) const;
        void DrawTriangle   (Device& device, const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, bool wireframe = false, const Colour& clr = Colour{ Colours::White }) const;
        void DrawQuad       (Device& device, const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, bool wireframe = false, const Colour& clr = Colour{ Colours::White }) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using PrimitiveBatch = std::unique_ptr<DirectX::PrimitiveBatch<Vertex>>;

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Colour          clearColour;
        bool            debugDraw[static_cast<int>(DebugDraw::TOTAL)];

        // Primitives
        PrimitiveBatch  primitiveBatch;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void renderAABB         (const Geometry::AABB& aabb, const View& v) const;
        void renderSphere       (Device& device, const Geometry::Sphere& sphere, const View& v, const Colour& clr = Colour{ Colours::Red }) const;

        void renderAABBTree     (const Geometry::AABBTree& tree, const View& v) const;
        void renderBSphereTree  (Device& device, const Geometry::BSphereTree& tree, const View& v) const;

        void renderOctree       (const Geometry::Octree& octree, const View& v) const;
        void renderOctreeData   (Device& device, const Geometry::Octree& octree, const View& v) const;
        void renderBSPTreeData  (Device& device, const Geometry::BSPTree& bspTree, const View& v) const;
    };
}
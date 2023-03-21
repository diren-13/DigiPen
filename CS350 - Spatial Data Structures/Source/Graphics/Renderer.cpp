/************************************************************************************//*!
\file           Renderer.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 13, 2022
\brief          Implementation of the Rendering system of the framework.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Renderer.h"
// Project Headers
#include "Graphics/Engine.h"
#include "Graphics/Graphics.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Renderer::Renderer(Device& device)
    : clearColour   { Colours::Black }
    , debugDraw     { false }
    {
        primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<Vertex>>(device.GetContext().Get());
    }

    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    bool Renderer::GetDebugDrawState(DebugDraw flag) const
    {
        return debugDraw[static_cast<int>(flag)];
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Renderer::SetDebugDrawState(DebugDraw flag, bool state)
    {
        debugDraw[static_cast<int>(flag)] = state;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void Renderer::RenderAxes(const View& v) const
    {
        const float FLT_HALF_EXTENTS = v.Camera->GetFarPlane();

        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.Colour                = Colour { 0.5f, 0.5f, 0.5f, 0.2f };
        vscBuffer.World                 = Mat4::Identity;
        vscBuffer.ViewProjection        = v.Camera->GetViewProjectionMatrix();

        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

        // Render Axes Based on the camera's far plane
        /// X-Axis
        vscBuffer.Colour = Colour{ Colours::DarkRed };
        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));
        primitiveBatch->Begin();
        {
            const Vec3 X_AXIS_MAX{ FLT_HALF_EXTENTS, 0.0f, 0.0f };
            const Vec3 X_AXIS_MIN{-FLT_HALF_EXTENTS, 0.0f, 0.0f };

            primitiveBatch->DrawLine(X_AXIS_MIN, X_AXIS_MAX);
        }
        primitiveBatch->End();

        /// Y-Axis
        vscBuffer.Colour = Colour{ Colours::DarkGreen };
        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));
        primitiveBatch->Begin();
        {
            const Vec3 Y_AXIS_MAX{ 0.0f, FLT_HALF_EXTENTS, 0.0f };
            const Vec3 Y_AXIS_MIN{ 0.0f,-FLT_HALF_EXTENTS, 0.0f };

            primitiveBatch->DrawLine(Y_AXIS_MIN, Y_AXIS_MAX);
        }
        primitiveBatch->End();

        /// Z-Axis
        vscBuffer.Colour = Colour{ Colours::DarkBlue };
        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));
        primitiveBatch->Begin();
        {
            const Vec3 Z_AXIS_MAX{ 0.0f, 0.0f, FLT_HALF_EXTENTS };
            const Vec3 Z_AXIS_MIN{ 0.0f, 0.0f,-FLT_HALF_EXTENTS };

            primitiveBatch->DrawLine(Z_AXIS_MIN, Z_AXIS_MAX);
        }
        primitiveBatch->End();
    }


    void Renderer::RenderGrid(const View& v) const
    {
        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.Colour                = Colour { Colours::LightSlateGray };
        vscBuffer.World                 = Mat4::Identity;
        vscBuffer.ViewProjection        = v.Camera->GetViewProjectionMatrix();

        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

        // Render a Grid Based on the camera's far plane
        const float FLT_HALF_EXTENTS = v.Camera->GetFarPlane();
        const int   INT_HALF_EXTENTS = static_cast<int>(FLT_HALF_EXTENTS);

        primitiveBatch->Begin();
        {
            
            for (int i = -INT_HALF_EXTENTS; i <= INT_HALF_EXTENTS; i += 5)
            {
                const Vec3 TOP      { static_cast<float>(i), 0.0f, FLT_HALF_EXTENTS };
                const Vec3 BOTTOM   { static_cast<float>(i), 0.0f,-FLT_HALF_EXTENTS };
                const Vec3 LEFT     { FLT_HALF_EXTENTS, 0.0f, static_cast<float>(i) };
                const Vec3 RIGHT    {-FLT_HALF_EXTENTS, 0.0f, static_cast<float>(i) };

                primitiveBatch->DrawLine(TOP, BOTTOM);
                primitiveBatch->DrawLine(RIGHT, LEFT);
            }
        }
        primitiveBatch->End();
    }

    void Renderer::Draw(Device& device, const Scene& scene, const View& v) const
    {
        v.Viewport->Set(device);

        // Render Drawables
        /// Specific handling for octree data for project 3
        const bool DRAW_DATA = debugDraw[static_cast<int>(DebugDraw::OctreeData)] || debugDraw[static_cast<int>(DebugDraw::BSPTreeData)];

        if (!DRAW_DATA)
        {
            const auto& DRAWABLES = scene.GetDrawables();
            for (auto& DRAWABLE : DRAWABLES)
            {
                DRAWABLE.Draw(device, *(v.Camera), scene.GetLights());

                if (debugDraw[static_cast<int>(DebugDraw::AABB)])
                    renderAABB(DRAWABLE.GetAABB(), v);

                if (debugDraw[static_cast<int>(DebugDraw::RitterSphere)])
                    renderSphere(device, DRAWABLE.GetRitterSphere(), v, Colour{ Colours::YellowGreen } );

                if (debugDraw[static_cast<int>(DebugDraw::LarssonSphere)])
                    renderSphere(device, DRAWABLE.GetLarssonSphere(), v, Colour{ Colours::Beige });

                if (debugDraw[static_cast<int>(DebugDraw::PCASphere)])
                    renderSphere(device, DRAWABLE.GetPCASphere(), v, Colour { Colours::SandyBrown});
            }
        }
        else
        {
            if (debugDraw[static_cast<int>(DebugDraw::OctreeData)])
                renderOctreeData(device, *scene.GetOctree(), v);

            if (debugDraw[static_cast<int>(DebugDraw::BSPTreeData)])
                renderBSPTreeData(device, *scene.GetBSPTree(), v);
        }

        if (debugDraw[static_cast<int>(DebugDraw::AABBTree)])
            renderAABBTree(*scene.GetAABBTree(), v);

        if (debugDraw[static_cast<int>(DebugDraw::RitterSphereTree)])
            renderBSphereTree(device, *scene.GetRitterTree(), v);

        if (debugDraw[static_cast<int>(DebugDraw::LarssonSphereTree)])
            renderBSphereTree(device, *scene.GetLarssonTree(), v);

        if (debugDraw[static_cast<int>(DebugDraw::PCASphereTree)])
            renderBSphereTree(device, *scene.GetPCATree(), v);

        if (debugDraw[static_cast<int>(DebugDraw::Octree)])
            renderOctree(*scene.GetOctree(), v);
    }

    void Renderer::DrawLine(Device& device, const View& view, const Vec3& p0, const Vec3& p1, const Colour& clr) const
    {
        if (!view.active)
            return;

        view.Viewport->Set(device);

        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.Colour            = clr;
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = view.Camera->GetViewProjectionMatrix();

        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

        primitiveBatch->Begin();
        {
            primitiveBatch->DrawLine(p0, p1);
        }
        primitiveBatch->End();
    }

    void Renderer::DrawTriangle(Device& device, const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, bool wireframe, const Colour& clr) const
    {
        if (!view.active)
            return;

        view.Viewport->Set(device);

        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.Colour            = clr;
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = view.Camera->GetViewProjectionMatrix();

        auto& material = wireframe ? Graphics::GetEngine()->GetWireframeMaterial() : Graphics::GetEngine()->GetSolidMaterial();
        material.Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

        primitiveBatch->Begin();
        {
            primitiveBatch->DrawTriangle(p0, p1, p2);
        }
        primitiveBatch->End();
    }

    void Renderer::DrawQuad(Device& device, const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, bool wireframe, const Colour& clr) const
    {
        if (!view.active)
            return;

        view.Viewport->Set(device);

        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.Colour            = clr;
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = view.Camera->GetViewProjectionMatrix();

        auto& material = wireframe ? Graphics::GetEngine()->GetWireframeMaterial() : Graphics::GetEngine()->GetSolidMaterial();
        material.Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

        primitiveBatch->Begin();
        {
            primitiveBatch->DrawQuad(p0, p1, p2, p3);
        }
        primitiveBatch->End();
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    void Renderer::renderSphere(Device& device, const Geometry::Sphere& sphere, const View& v, const Colour& clr) const
    {
        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.Colour            = clr;
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = v.Camera->GetViewProjectionMatrix();

        const Mat4 TRANSLATE    = Mat4::CreateTranslation(sphere.GetCenter());
        const Mat4 SCALE        = Mat4::CreateScale(sphere.GetRadius() * 2.0f);

        vscBuffer.World = SCALE * TRANSLATE;

        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));
        Graphics::GetEngine()->GetSphere().Draw(device);
    }


    void Renderer::renderAABB(const Geometry::AABB& aabb, const View& v) const
    {
        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.Colour            = Colour { Colours::Green };
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = v.Camera->GetViewProjectionMatrix();

        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

        const Vec3 MIN = aabb.GetMin();
        const Vec3 MAX = aabb.GetMax();

        const Vec3 V1{ MIN.x, MAX.y, MAX.z };
        const Vec3 V2{ MIN.x, MAX.y, MIN.z };
        const Vec3 V3{ MAX.x, MAX.y, MIN.z };
        const Vec3 V4{ MIN.x, MIN.y, MAX.z };
        const Vec3 V5{ MAX.x, MIN.y, MIN.z };
        const Vec3 V6{ MAX.x, MIN.y, MAX.z };

        primitiveBatch->Begin();
        {
            // Front Face
            primitiveBatch->DrawLine(MIN, V5);
            primitiveBatch->DrawLine(V2,  V3);
            primitiveBatch->DrawLine(MIN, V2);
            primitiveBatch->DrawLine(V5,  V3);

            // Back Face
            primitiveBatch->DrawLine(V4,  V6);
            primitiveBatch->DrawLine(V1, MAX);
            primitiveBatch->DrawLine(V4,  V1);
            primitiveBatch->DrawLine(V6, MAX);

            // Left Face
            primitiveBatch->DrawLine(MIN, V4);
            primitiveBatch->DrawLine(V2,  V1);

            // Right Face
            primitiveBatch->DrawLine(V5,  V6);
            primitiveBatch->DrawLine(V3, MAX);
        }
        primitiveBatch->End();
    }

    void Renderer::renderAABBTree(const Geometry::AABBTree& tree, const View& v) const
    {
        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = v.Camera->GetViewProjectionMatrix();


        const auto NODES = tree.GetNodes();
        for (const auto& node : NODES)
        {
            switch (node.second)
            {
                case 0  : vscBuffer.Colour = Colour{ 141.0f / 255.0f, 211.0f / 255.0f, 199.0f / 255.0f };   break;
                case 1  : vscBuffer.Colour = Colour{ 255.0f / 255.0f, 255.0f / 255.0f, 179.0f / 255.0f };   break;
                case 2  : vscBuffer.Colour = Colour{ 190.0f / 255.0f, 186.0f / 255.0f, 218.0f / 255.0f };   break;
                case 3  : vscBuffer.Colour = Colour{ 251.0f / 255.0f, 128.0f / 255.0f, 114.0f / 255.0f };   break;
                case 4  : vscBuffer.Colour = Colour{ 128.0f / 255.0f, 177.0f / 255.0f, 211.0f / 255.0f };   break;
                case 5  : vscBuffer.Colour = Colour{ 253.0f / 255.0f, 180.0f / 255.0f,  98.0f / 255.0f };   break;
                default : vscBuffer.Colour = Colour{ 179.0f / 255.0f, 222.0f / 255.0f, 105.0f / 255.0f };   break;
            }

            Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

            const Vec3 MIN = node.first.GetMin();
            const Vec3 MAX = node.first.GetMax();

            const Vec3 V1{ MIN.x, MAX.y, MAX.z };
            const Vec3 V2{ MIN.x, MAX.y, MIN.z };
            const Vec3 V3{ MAX.x, MAX.y, MIN.z };
            const Vec3 V4{ MIN.x, MIN.y, MAX.z };
            const Vec3 V5{ MAX.x, MIN.y, MIN.z };
            const Vec3 V6{ MAX.x, MIN.y, MAX.z };

            primitiveBatch->Begin();
            {
                // Front Face
                primitiveBatch->DrawLine(MIN, V5);
                primitiveBatch->DrawLine(V2,  V3);
                primitiveBatch->DrawLine(MIN, V2);
                primitiveBatch->DrawLine(V5,  V3);

                // Back Face
                primitiveBatch->DrawLine(V4,  V6);
                primitiveBatch->DrawLine(V1, MAX);
                primitiveBatch->DrawLine(V4,  V1);
                primitiveBatch->DrawLine(V6, MAX);

                // Left Face
                primitiveBatch->DrawLine(MIN, V4);
                primitiveBatch->DrawLine(V2,  V1);

                // Right Face
                primitiveBatch->DrawLine(V5,  V6);
                primitiveBatch->DrawLine(V3, MAX);
            }
            primitiveBatch->End();
        }
    }

    void Renderer::renderBSphereTree(Device& device, const Geometry::BSphereTree& tree, const View& v) const
    {
        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = v.Camera->GetViewProjectionMatrix();

        const auto NODES = tree.GetNodes();
        const Geometry::Sphere::Method SPHERE_METHOD = NODES.front().first.GetMethod();
        for (const auto& node : NODES)
        {
            if (SPHERE_METHOD == Geometry::Sphere::Method::Ritter)
            {
                switch (node.second)
                {
                    case 0  : vscBuffer.Colour = Colour{ 127.0f / 255.0f, 201.0f / 255.0f, 127.0f / 255.0f };   break;
                    case 1  : vscBuffer.Colour = Colour{ 190.0f / 255.0f, 174.0f / 255.0f, 212.0f / 255.0f };   break;
                    case 2  : vscBuffer.Colour = Colour{ 253.0f / 255.0f, 192.0f / 255.0f, 134.0f / 255.0f };   break;
                    case 3  : vscBuffer.Colour = Colour{ 255.0f / 255.0f, 255.0f / 255.0f, 153.0f / 255.0f };   break;
                    case 4  : vscBuffer.Colour = Colour{  56.0f / 255.0f, 108.0f / 255.0f, 176.0f / 255.0f };   break;
                    case 5  : vscBuffer.Colour = Colour{ 240.0f / 255.0f,   2.0f / 255.0f, 127.0f / 255.0f };   break;
                    default : vscBuffer.Colour = Colour{ 191.0f / 255.0f,  91.0f / 255.0f,  23.0f / 255.0f };   break;
                }
            }
            else if (SPHERE_METHOD == Geometry::Sphere::Method::PCA)
            {
                switch (node.second)
                {
                    case 0  : vscBuffer.Colour = Colour{ 166.0f / 255.0f, 206.0f / 255.0f, 227.0f / 255.0f };   break;
                    case 1  : vscBuffer.Colour = Colour{  31.0f / 255.0f, 120.0f / 255.0f, 180.0f / 255.0f };   break;
                    case 2  : vscBuffer.Colour = Colour{ 178.0f / 255.0f, 223.0f / 255.0f, 138.0f / 255.0f };   break;
                    case 3  : vscBuffer.Colour = Colour{  51.0f / 255.0f, 160.0f / 255.0f,  44.0f / 255.0f };   break;
                    case 4  : vscBuffer.Colour = Colour{ 251.0f / 255.0f, 154.0f / 255.0f, 153.0f / 255.0f };   break;
                    case 5  : vscBuffer.Colour = Colour{ 227.0f / 255.0f,  26.0f / 255.0f,  28.0f / 255.0f };   break;
                    default : vscBuffer.Colour = Colour{ 253.0f / 255.0f, 191.0f / 255.0f, 111.0f / 255.0f };   break;
                }
            }
            else
            {
                switch (node.second)
                {
                    case 0  : vscBuffer.Colour = Colour{ 179.0f / 255.0f,  88.0f / 255.0f,   6.0f / 255.0f };   break;
                    case 1  : vscBuffer.Colour = Colour{ 241.0f / 255.0f, 163.0f / 255.0f,  64.0f / 255.0f };   break;
                    case 2  : vscBuffer.Colour = Colour{ 254.0f / 255.0f, 224.0f / 255.0f, 182.0f / 255.0f };   break;
                    case 3  : vscBuffer.Colour = Colour{ 247.0f / 255.0f, 247.0f / 255.0f, 247.0f / 255.0f };   break;
                    case 4  : vscBuffer.Colour = Colour{ 216.0f / 255.0f, 218.0f / 255.0f, 235.0f / 255.0f };   break;
                    case 5  : vscBuffer.Colour = Colour{ 153.0f / 255.0f, 142.0f / 255.0f, 195.0f / 255.0f };   break;
                    default : vscBuffer.Colour = Colour{  84.0f / 255.0f,  39.0f / 255.0f, 136.0f / 255.0f };   break;
                }
            }

            const Mat4 TRANSLATE    = Mat4::CreateTranslation(node.first.GetCenter());
            const Mat4 SCALE        = Mat4::CreateScale(node.first.GetRadius() * 2.0f);

            vscBuffer.World = SCALE * TRANSLATE;

            Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));
            Graphics::GetEngine()->GetSphere().Draw(device);
        }
    }

    void Renderer::renderOctree(const Geometry::Octree& octree, const View& v) const
    {
        DefaultPSCBuffer psCBuffer{};
        psCBuffer.DirectionalLightCount = 0;

        DefaultVSCBuffer vscBuffer{};
        vscBuffer.World             = Mat4::Identity;
        vscBuffer.ViewProjection    = v.Camera->GetViewProjectionMatrix();
        vscBuffer.Colour            = Colour{ Colours::CadetBlue };

        Graphics::GetEngine()->GetWireframeMaterial().Set(&vscBuffer, sizeof(DefaultVSCBuffer), &psCBuffer, sizeof(DefaultPSCBuffer));

        const auto NODES = octree.GetRenderNodes();
        for (const auto& node : NODES)
        {
            const Vec3 MIN = node.first.GetMin();
            const Vec3 MAX = node.first.GetMax();

            const Vec3 V1{ MIN.x, MAX.y, MAX.z };
            const Vec3 V2{ MIN.x, MAX.y, MIN.z };
            const Vec3 V3{ MAX.x, MAX.y, MIN.z };
            const Vec3 V4{ MIN.x, MIN.y, MAX.z };
            const Vec3 V5{ MAX.x, MIN.y, MIN.z };
            const Vec3 V6{ MAX.x, MIN.y, MAX.z };

            primitiveBatch->Begin();
            {
                // Front Face
                primitiveBatch->DrawLine(MIN, V5);
                primitiveBatch->DrawLine(V2,  V3);
                primitiveBatch->DrawLine(MIN, V2);
                primitiveBatch->DrawLine(V5,  V3);

                // Back Face
                primitiveBatch->DrawLine(V4,  V6);
                primitiveBatch->DrawLine(V1, MAX);
                primitiveBatch->DrawLine(V4,  V1);
                primitiveBatch->DrawLine(V6, MAX);

                // Left Face
                primitiveBatch->DrawLine(MIN, V4);
                primitiveBatch->DrawLine(V2,  V1);

                // Right Face
                primitiveBatch->DrawLine(V5,  V6);
                primitiveBatch->DrawLine(V3, MAX);
            }
            primitiveBatch->End();
        }
    }

    void Renderer::renderOctreeData(Device& device, const Geometry::Octree& octree, const View& v) const
    {
        // Every 3 points in Octree Data is a triangle
        const auto NODES = octree.GetDataNodes();
        for (const auto& node : NODES)
        {
            Colour triColour;

            switch (node.second)
            {
                case 0  : triColour = Colour{ 166.0f / 255.0f,   206.0f / 255.0f,    227.0f / 255.0f };   break;
                case 1  : triColour = Colour{  31.0f / 255.0f,   120.0f / 255.0f,    180.0f / 255.0f };   break;
                case 2  : triColour = Colour{ 178.0f / 255.0f,   223.0f / 255.0f,    138.0f / 255.0f };   break;
                case 3  : triColour = Colour{  51.0f / 255.0f,   160.0f / 255.0f,     44.0f / 255.0f };   break;
                case 4  : triColour = Colour{ 251.0f / 255.0f,   154.0f / 255.0f,    153.0f / 255.0f };   break;
                case 5  : triColour = Colour{ 227.0f / 255.0f,    26.0f / 255.0f,     28.0f / 255.0f };   break;
                case 6  : triColour = Colour{ 253.0f / 255.0f,   191.0f / 255.0f,    111.0f / 255.0f };   break;
                case 7  : triColour = Colour{ 255.0f / 255.0f,   127.0f / 255.0f,      0.0f / 255.0f };   break;
                case 8  : triColour = Colour{ 202.0f / 255.0f,   178.0f / 255.0f,    214.0f / 255.0f };   break;
                case 9  : triColour = Colour{ 106.0f / 255.0f,    61.0f / 255.0f,    154.0f / 255.0f };   break;
                case 10 : triColour = Colour{ 255.0f / 255.0f,   255.0f / 255.0f,    153.0f / 255.0f };   break;
                case 11 : triColour = Colour{ 177.0f / 255.0f,    89.0f / 255.0f,     40.0f / 255.0f };   break;
                default : triColour = Colour{ 179.0f / 255.0f,   222.0f / 255.0f,    105.0f / 255.0f };   break;
            }

            const auto& TRIANGLES = node.first->Vertices;
            if (TRIANGLES.empty())
                continue;

            for (size_t i = 0; i < TRIANGLES.size(); i += 3)
            {
                const Vec3& P0 = TRIANGLES[i];
                const Vec3& P1 = TRIANGLES[i + 1];
                const Vec3& P2 = TRIANGLES[i + 2];

                DrawTriangle(device, v, P0, P1, P2, true, triColour);
            }
        }
    }

    void Renderer::renderBSPTreeData(Device& device, const Geometry::BSPTree& bspTree, const View& v) const
    {
        // Every 3 points in BSPTree Data is a triangle
        const auto NODES = bspTree.GetDataNodes();
        for (const auto& node : NODES)
        {
            Colour triColour;

            switch (node.second % 20)
            {
                case 0  : triColour = Colour{ 166.0f / 255.0f,   206.0f / 255.0f,    227.0f / 255.0f };   break;
                case 1  : triColour = Colour{  31.0f / 255.0f,   120.0f / 255.0f,    180.0f / 255.0f };   break;
                case 2  : triColour = Colour{ 178.0f / 255.0f,   223.0f / 255.0f,    138.0f / 255.0f };   break;
                case 3  : triColour = Colour{  51.0f / 255.0f,   160.0f / 255.0f,     44.0f / 255.0f };   break;
                case 4  : triColour = Colour{ 251.0f / 255.0f,   154.0f / 255.0f,    153.0f / 255.0f };   break;
                case 5  : triColour = Colour{ 227.0f / 255.0f,    26.0f / 255.0f,     28.0f / 255.0f };   break;
                case 6  : triColour = Colour{ 253.0f / 255.0f,   191.0f / 255.0f,    111.0f / 255.0f };   break;
                case 7  : triColour = Colour{ 255.0f / 255.0f,   127.0f / 255.0f,      0.0f / 255.0f };   break;
                case 8  : triColour = Colour{ 202.0f / 255.0f,   178.0f / 255.0f,    214.0f / 255.0f };   break;
                case 9  : triColour = Colour{ 106.0f / 255.0f,    61.0f / 255.0f,    154.0f / 255.0f };   break;
                case 10 : triColour = Colour{ 255.0f / 255.0f,   255.0f / 255.0f,    153.0f / 255.0f };   break;
                case 11 : triColour = Colour{ 177.0f / 255.0f,    89.0f / 255.0f,     40.0f / 255.0f };   break;
                case 12 : triColour = Colour{ 179.0f / 255.0f,    88.0f / 255.0f,      6.0f / 255.0f };   break;
                case 13 : triColour = Colour{ 241.0f / 255.0f,   163.0f / 255.0f,     64.0f / 255.0f };   break;
                case 14 : triColour = Colour{ 254.0f / 255.0f,   224.0f / 255.0f,    182.0f / 255.0f };   break;
                case 15 : triColour = Colour{ 247.0f / 255.0f,   247.0f / 255.0f,    247.0f / 255.0f };   break;
                case 16 : triColour = Colour{ 216.0f / 255.0f,   218.0f / 255.0f,    235.0f / 255.0f };   break;
                case 17 : triColour = Colour{ 153.0f / 255.0f,   142.0f / 255.0f,    195.0f / 255.0f };   break;
                case 18 : triColour = Colour{  84.0f / 255.0f,    39.0f / 255.0f,    136.0f / 255.0f };   break;
                case 19 : triColour = Colour{ 166.0f / 255.0f,   206.0f / 255.0f,    227.0f / 255.0f };   break;
                default : triColour = Colour{ 179.0f / 255.0f,   222.0f / 255.0f,    105.0f / 255.0f };   break;
            }

            const auto& TRIANGLES = node.first->Vertices;
            if (TRIANGLES.empty())
                continue;

            for (size_t i = 0; i < TRIANGLES.size(); i += 3)
            {
                const Vec3& P0 = TRIANGLES[i];
                const Vec3& P1 = TRIANGLES[i + 1];
                const Vec3& P2 = TRIANGLES[i + 2];

                DrawTriangle(device, v, P0, P1, P2, true, triColour);
            }
        }
    }
}

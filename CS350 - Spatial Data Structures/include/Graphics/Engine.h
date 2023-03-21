/************************************************************************************//*!
\file           Engine.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 13, 2022
\brief          Encapsulation of the Engine which handles resources and rendering.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Project Headers
#include "Graphics.h"
#include "Device.h"
#include "Renderer.h"
#include "Scene.h"
#include "Editor/Editor.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief    Encapsulates the Engine for the Clam Chowder Rendering Framework.
    *//*********************************************************************************/
    class Engine
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] Renderer&         GetRenderer         () const;
        [[nodiscard]] Scene&            GetCurrentScene     () const;

        [[nodiscard]] Device&           GetDevice           ();

        [[nodiscard]] Material*         GetMaterial         (const std::string& name);
        [[nodiscard]] Model*            GetModel            (const std::string& name);
        [[nodiscard]] Viewport*         GetViewport         (const std::string& name);
        [[nodiscard]] Camera*           GetCamera           (const std::string& name);

        // Default Models
        [[nodiscard]] Model&            GetTriangle         ();
        [[nodiscard]] Model&            GetQuad             ();
        [[nodiscard]] Model&            GetSphere           ();
        [[nodiscard]] Model&            GetCube             ();

        // Default Materials
        [[nodiscard]] Material&         GetSolidMaterial    ();
        [[nodiscard]] Material&         GetWireframeMaterial();

        // Default Views
        [[nodiscard]] View&             GetDefaultView      ();
        [[nodiscard]] View&             GetMinimapView      ();

        [[nodiscard]] bool              GetRenderDefaultGrid()  const   { return defaultViewGrid; }
        [[nodiscard]] bool              GetRenderDefaultAxes()  const   { return defaultViewAxes; }
        [[nodiscard]] bool              GetRenderMinimapAxes()  const   { return minimapAxes; }
        [[nodiscard]] bool              GetRenderMinimapFOV ()  const   { return minimapFOV; }
        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetCurrentScene        (Scene& scene);

        void SetRenderDefaultGrid   (bool state)    { defaultViewGrid = state; }
        void SetRenderDefaultAxes   (bool state)    { defaultViewAxes = state; }
        void SetRenderMinimapAxes   (bool state)    { minimapAxes = state; }
        void SetRenderMinimapFOV    (bool state)    { minimapFOV = state; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        Material&   CreateMaterial  (const std::string& name, const MaterialSetup& setup);

        Model&      CreateModel     (const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        Model&      CreateModel     (const std::string& name, const Vertex* vertices, unsigned int numVertices, const uint32_t* indices, unsigned int numIndices);
        Model&      LoadModel       (const std::string& name, const std::filesystem::path& filePath);

        Viewport&   CreateViewport  (const std::string& name, const Vec2& topLeft, float width, float height, float minDepth, float maxDepth);

        Camera&     CreateCamera    (const std::string& name);

        // Runtime
        void        BeginRendering  ();
        void        Render          (Editor& editor, std::vector<View>& views);
        void        RenderLine      (const View& view, const Vec3& p0, const Vec3& p1, const Colour& clr = Colour{ Colours::White });
        void        RenderTriangle  (const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Colour& clr = Colour{ Colours::White });
        void        RenderQuad      (const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, bool wireframe = false, const Colour& clr = Colour{ Colours::White });
        void        EndRendering    ();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        struct FullScreenQuad
        {
            struct Vertex2D
            {
                Vec2    position;
                Colour  colour;
            };

            Material*               material = nullptr;
            VertexBuffer<Vertex2D>  vBuffer;
            IndexBuffer<uint32_t>   iBuffer;
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Engine  (Window* _window);
        ~Engine ();

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Window*                     window;             // Also used as a boolean to indicate the engine successfully initialised
        Renderer*                   renderer;
        Scene*                      currentScene;

        FullScreenQuad              fullscreenQuad;
        FullScreenQuad              minimapQuad;

        Device                      device;
        Graphics::Swapchain         swapChain;          // Swap frames while rendering (we discard frames)

        // Views
        Graphics::RenderTargetView  renderTargetView;   // Where to render the buffer
        Graphics::DepthStencilView  depthStencilView;
        View                        defaultView;
        bool                        defaultViewGrid;
        bool                        defaultViewAxes;
        View                        minimapView;
        bool                        minimapAxes;
        bool                        minimapFOV;

        // Containers
        Graphics::Materials         materials;
        Graphics::Models            models;
        Graphics::Viewports         viewports;
        Graphics::Cameras           cameras;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        bool initialise             ();

        void createFullScreenQuad   ();
        void createDefaultView      ();
        void createMinimapView      ();
        void createDefaultMaterial  ();
        void createTriangle         ();
        void createQuad             ();
        void createSphere           ();
        void createCube             ();

        bool resizeSwapchain        ();
        void recreateViewports      ();

        void renderFullScreenQuad   ();
        void renderDefault          ();
        void updateMinimapCam       ();
        void renderMinimap          ();

        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class Graphics;
    };
}

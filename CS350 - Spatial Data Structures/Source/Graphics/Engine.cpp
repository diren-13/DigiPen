/************************************************************************************//*!
\file           Engine.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Implementation of the Engine which handles resources and rendering.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Engine.h"
// DirectX Headers
#include <GeometricPrimitive.h>
// Project Headers
#include "Graphics/Adapter.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    

    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/

    Engine::Engine(Window* _window)
    : window            { _window }
    , renderer          { nullptr }
    , currentScene      { nullptr }
    , defaultViewGrid   { true }
    , defaultViewAxes   { true }
    , minimapAxes       { true }
    , minimapFOV        { true }
    {
        if (!window)
        {
            Log(LogSeverity::Error, "Window is invalid!");
            return;
        }

        const bool INIT_RESULT = initialise();
        if (!INIT_RESULT)
        {
            Log(LogSeverity::Error, "Failed to Initialise the engine!");
            window = nullptr;
            return;
        }

        renderer = new Renderer{ device };

        // Default Resource Creation
        createDefaultMaterial();
        createFullScreenQuad();
        createDefaultView();
        createMinimapView();

        createTriangle();
        createQuad();
        createSphere();
        createCube();
    }

    Engine::~Engine()
    {
        delete fullscreenQuad.material;
        delete renderer;
    }


    /*---------------------------------------------------------------------------------*/
    /* Getter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    Renderer& Engine::GetRenderer() const
    {
        return *renderer;
    }

    Scene& Engine::GetCurrentScene() const
    {
        return *currentScene;
    }

    Device& Engine::GetDevice()
    {
        return device;
    }


    Material* Engine::GetMaterial(const std::string& name)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = materials.find(ID);
        if (IT == materials.end())
        {
            Log(LogSeverity::Error, "Material cannot be found!");
            return nullptr;
        }

        return &IT->second;
    }

    Model* Engine::GetModel(const std::string& name)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = models.find(ID);
        if (IT == models.end())
        {
            Log(LogSeverity::Error, "Model cannot be found!");
            return nullptr;
        }

        return &IT->second;
    }

    Viewport* Engine::GetViewport(const std::string& name)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = viewports.find(ID);
        if (IT == viewports.end())
        {
            Log(LogSeverity::Error, "Viewport cannot be found!");
            return nullptr;
        }

        return &IT->second;
    }

    Camera* Engine::GetCamera(const std::string& name)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = cameras.find(ID);
        if (IT == cameras.end())
        {
            Log(LogSeverity::Error, "Camera cannot be found!");
            return nullptr;
        }

        return &IT->second;
    }

    Model& Engine::GetTriangle()
    {
        const size_t ID = Graphics::StringHash("Triangle");
        return models.find(ID)->second;
    }

    Model& Engine::GetQuad()
    {
        const size_t ID = Graphics::StringHash("Quad");
        return models.find(ID)->second;
    }

    Model& Engine::GetSphere()
    {
        const size_t ID = Graphics::StringHash("Sphere");
        return models.find(ID)->second;
    }

    Model& Engine::GetCube()
    {
        const size_t ID = Graphics::StringHash("Cube");
        return models.find(ID)->second;
    }

    Material& Engine::GetSolidMaterial()
    {
        const size_t ID = Graphics::StringHash("Solid");
        return materials.find(ID)->second;
    }

    Material& Engine::GetWireframeMaterial()
    {
        const size_t ID = Graphics::StringHash("Wireframe");
        return materials.find(ID)->second;
    }

    View& Engine::GetDefaultView()
    {
        return defaultView;
    }

    View& Engine::GetMinimapView()
    {
        return minimapView;
    }



    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Engine::SetCurrentScene(Scene& scene)
    {
        currentScene = &scene;
        currentScene->Build();
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    Material& Engine::CreateMaterial(const std::string& name, const MaterialSetup& setup)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = materials.find(ID);
        if (IT != materials.end())
        {
            Log(LogSeverity::Warning, "Material with the same name has already been created!");
            return IT->second;
        }

        const auto RETVAL = materials.emplace(ID, Material{ device, setup });
        return RETVAL.first->second;
    }

    Model& Engine::CreateModel(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = models.find(ID);
        if (IT != models.end())
        {
            Log(LogSeverity::Warning, "Model with the same name has already been created!");
            return IT->second;
        }

        const auto RETVAL = models.emplace(ID, Model{ device, vertices, indices });
        return RETVAL.first->second;
    }

    Model& Engine::CreateModel(const std::string& name, const Vertex* vertices, unsigned numVertices, const uint32_t* indices, unsigned numIndices)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = models.find(ID);
        if (IT != models.end())
        {
            Log(LogSeverity::Warning, "Model with the same name has already been created!");
            return IT->second;
        }

        const auto RETVAL = models.emplace(ID, Model{ device, vertices, numVertices, indices, numIndices });
        return RETVAL.first->second;
    }

    Model& Engine::LoadModel(const std::string& name, const std::filesystem::path& filePath)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = models.find(ID);
        if (IT != models.end())
        {
            Log(LogSeverity::Warning, "Model with the same name has already been created!");
            return IT->second;
        }

        const auto RETVAL = models.emplace(ID, Model{ device, filePath });
        return RETVAL.first->second;
    }


    Viewport& Engine::CreateViewport(const std::string& name, const Vec2& topLeft, float width, float height, float minDepth, float maxDepth)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = viewports.find(ID);
        if (IT != viewports.end())
        {
            Log(LogSeverity::Warning, "Viewport with the same name has already been created!");
            return IT->second;
        }

        const auto RETVAL = viewports.emplace(ID, Viewport{ topLeft.x, topLeft.y, width, height, minDepth, maxDepth });
        return RETVAL.first->second;
    }

    Camera& Engine::CreateCamera(const std::string& name)
    {
        const size_t ID = Graphics::StringHash(name);

        const auto IT = cameras.find(ID);
        if (IT != cameras.end())
        {
            Log(LogSeverity::Warning, "Camera with the same name has already been created!");
            return IT->second;
        }

        const auto RETVAL = cameras.emplace(ID, Camera{});
        return RETVAL.first->second;
    }

    void Engine::BeginRendering()
    {
        device.GetContext()->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

        // Clear render target view
        device.GetContext()->ClearRenderTargetView(renderTargetView.Get(), renderer->GetClearColour());
        device.GetContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    void Engine::Render(Editor& editor, std::vector<View>& views)
    {
        if (window->GetMinimised())
            return;

        if (!currentScene)
            return;

        if (window->GetResized())
        {
            if (!resizeSwapchain())
                return;
        }

        BeginRendering();
        {
            currentScene->Update();

            renderDefault();
            for (auto& v : views)
            {
                if (!v.active)
                    continue;

                renderer->Draw(device, *currentScene, v);
            }
            renderMinimap();
            editor.Render();
        }
        EndRendering();
        
    }

    void Engine::RenderLine(const View& view, const Vec3& p0, const Vec3& p1, const Colour& clr)
    {
        renderer->DrawLine(device, view, p0, p1, clr);
    }

    void Engine::RenderTriangle(const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Colour& clr)
    {
        renderer->DrawTriangle(device, view, p0, p1, p2, clr);
    }


    void Engine::RenderQuad(const View& view, const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, bool wireframe, const Colour& clr)
    {
        renderer->DrawQuad(device, view, p0, p1, p2, p3, wireframe, clr);
    }

    void Engine::EndRendering()
    {
        swapChain->Present(1U, NULL);
    }


    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    bool Engine::initialise()
    {
        auto adapters = Adapter::GetAdapters();
        if (adapters.empty())
        {
            Log(LogSeverity::Error, "Failed to find a valid DXGI Adapter.");
            return false;
        }

        UINT d3dFlags{};
        d3dFlags |= D3D11_CREATE_DEVICE_DEBUG;

        // Setup Swapchain description
        DXGI_MODE_DESC bufferDesc{ 0 };
        bufferDesc.Width                    = static_cast<UINT>(window->GetWidth());
        bufferDesc.Height                   = static_cast<UINT>(window->GetHeight());
        bufferDesc.RefreshRate.Numerator    = 60U;                                      // Caps the max RR in FS with Vsync turned on
        bufferDesc.RefreshRate.Denominator  = 1U;
        bufferDesc.Format                   = DXGI_FORMAT_R8G8B8A8_UNORM;
        bufferDesc.ScanlineOrdering         = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        bufferDesc.Scaling                  = DXGI_MODE_SCALING_UNSPECIFIED;

        DXGI_SAMPLE_DESC sampleDesc{ 0 };
        sampleDesc.Count    = 1U;
        sampleDesc.Quality  = 0U;

        DXGI_SWAP_CHAIN_DESC swapChainDesc{ 0 };
        swapChainDesc.BufferDesc    = bufferDesc;
        swapChainDesc.SampleDesc    = sampleDesc;
        swapChainDesc.BufferUsage   = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount   = 1U;                                   // Has double buffering
        swapChainDesc.OutputWindow  = window->GetHandle();
        swapChainDesc.Windowed      = true;
        swapChainDesc.SwapEffect    = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags         = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        HRESULT hresult = D3D11CreateDeviceAndSwapChain
        (
            adapters.front().GetHandle().Get()      // Take the first adapter
        ,   D3D_DRIVER_TYPE_UNKNOWN                 // Unspecified
        ,   nullptr                                 // No software driver
        ,   d3dFlags                                // Flags for runtime layers
        ,   nullptr                                 // Feature levels for array
        ,   NULL                                    // Num feature levels in array
        ,   D3D11_SDK_VERSION
        ,   &swapChainDesc
        ,   swapChain.GetAddressOf()
        ,   device.GetHandle().GetAddressOf()
        ,   nullptr
        ,   device.GetContext().GetAddressOf()
        );
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create DirectX Device & Swapchain!");
            return false;
        }

        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created DirectX Device & Swapchain.");
        #endif

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        hresult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to get Swapchain's back buffer! Check if swapchain was successfully created!");
            return false;
        }

        hresult = device.GetHandle()->CreateRenderTargetView
        (
            backBuffer.Get()
        ,   nullptr
        ,   renderTargetView.GetAddressOf()
        );
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create a Render Target!");
            return false;
        }

        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Render Target.");
        #endif

        D3D11_TEXTURE2D_DESC depthStencilDesc { 0 };
        depthStencilDesc.Width              = static_cast<UINT>(window->GetWidth());
        depthStencilDesc.Height             = static_cast<UINT>(window->GetHeight());
        depthStencilDesc.MipLevels          = 1U;
        depthStencilDesc.ArraySize          = 1U;
        depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count   = 1U;
        depthStencilDesc.SampleDesc.Quality = 0U;
        depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags     = 0U;
        depthStencilDesc.MiscFlags          = 0U;

        Graphics::Texture2D depthStencilBuffer;
        hresult = device.GetHandle()->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilBuffer.GetAddressOf());
        if (FAILED(hresult)) //If error occurred
        {
            Log(LogSeverity::Error, "Failed to create depth stencil buffer.");
            return false;
        }

        hresult = device.GetHandle()->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
        if (FAILED(hresult)) //If error occurred
        {
            Log(LogSeverity::Error, "Failed to create depth stencil view.");
            return false;
        }

        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Depth-Stencil View.");
        #endif

        return true;
    }

    void Engine::createFullScreenQuad()
    {
        #ifdef _DEBUG
            std::string defaultShaderPath = "build/Debug/";
        #else
            std::string defaultShaderPath = "build/Release/";
        #endif

        // Full Screen Quad Material
        const std::vector<D3D11_INPUT_ELEMENT_DESC> FSQUAD_LAYOUT_DESC =
        {
            {"POSITION",    0, DXGI_FORMAT_R32G32_FLOAT,        0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOUR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        MaterialSetup fsQuadMaterial{};
        fsQuadMaterial.VertexShaderPath         = defaultShaderPath + "QuadVS.cso";
        fsQuadMaterial.VertexLayoutDescription  = FSQUAD_LAYOUT_DESC;
        fsQuadMaterial.PixelShaderPath          = defaultShaderPath + "QuadPS.cso";
        fsQuadMaterial.VSCBufferSize            = 0;
        fsQuadMaterial.PSCBufferSize            = 0;
        fsQuadMaterial.FillMode                 = FillMode::Solid;
        fsQuadMaterial.CullMode                 = CullMode::None;
        fsQuadMaterial.DepthTesting             = false;

        fullscreenQuad.material = new Material{ device, fsQuadMaterial };

        const std::vector fsQuadVertices =
        {
            FullScreenQuad::Vertex2D{ Vec2{ 1.0f,  1.0f}, Colour{ 0.49f, 0.56f, 0.65f, 1.0f } },
            FullScreenQuad::Vertex2D{ Vec2{ 1.0f, -1.0f}, Colour{ 0.06f, 0.06f, 0.08f, 1.0f } },
            FullScreenQuad::Vertex2D{ Vec2{-1.0f, -1.0f}, Colour{ 0.06f, 0.06f, 0.08f, 1.0f } },
            FullScreenQuad::Vertex2D{ Vec2{-1.0f,  1.0f}, Colour{ 0.49f, 0.56f, 0.65f, 1.0f } }
        };
        const std::vector<uint32_t> fsQuadIndices = { 0, 1, 2, 2, 3, 0 };

        fullscreenQuad.vBuffer.Initialise(device, fsQuadVertices);
        fullscreenQuad.iBuffer.Initialise(device, fsQuadIndices);
    }

    void Engine::createDefaultView()
    {
        // Setup a default camera and viewport
        Camera& cam = CreateCamera("DefaultCam");
        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Camera: \"DefaultCam\"");
        #endif

        cam.SetPosition(Vec3{ 0.0f, 0.0f, -2.0f });
        cam.SetPerspective(window->GetAspectRatio());

        Viewport defaultViewport{ 0.0f, 0.0f };
        Viewport& vp = CreateViewport
        (
            "DefaultViewport", 
            Vec2::Zero, 
            static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()), 
            0.0f, 1.0f
        );
        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Viewport: \"DefaultViewport\"");
        #endif

        defaultView.Camera      = &cam;
        defaultView.Viewport    = &vp;
        defaultView.active      = true;
    }

    void Engine::createMinimapView()
    {
        // Minimap View
        Camera& minimapCam = CreateCamera("MinimapCam");
        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Camera: \"MinimapCam\"");
        #endif

        minimapCam.SetPosition(Vec3{ 0.0f, 7.5f, defaultView.Camera->GetPosition().z });

        const float Y_POS     = minimapCam.GetPosition().y;

        minimapCam.SetOrthographic(-Y_POS, Y_POS, Y_POS, -Y_POS);
        minimapCam.LookAt(Vec3::Zero);

        /// Minimap Viewport
        const float WINDOW_WIDTH    = static_cast<float>(window->GetWidth());
        const float WINDOW_HEIGHT   = static_cast<float>(window->GetHeight());

        const Vec2  PADDING     { 10.0f };
        const Vec2  MINIMAP_DIM { 144.0f,  144.0f };
        const Vec2  TOP_LEFT    { WINDOW_WIDTH - PADDING.x - MINIMAP_DIM.x, PADDING.y };

        Viewport& minimapVP = CreateViewport
        (
            "Minimap", TOP_LEFT, 
            MINIMAP_DIM.x, MINIMAP_DIM.y,
            defaultView.Viewport->GetMinDepth(),
            defaultView.Viewport->GetMaxDepth()
        );
        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Viewport: \"Minimap\"");
        #endif

        minimapView.Camera      = &minimapCam;
        minimapView.Viewport    = &minimapVP;
        minimapView.active      = true;

        // minimap quad
         const std::vector mmQuadVertices =
        {
            FullScreenQuad::Vertex2D{ Vec2{ 1.0f,  1.0f}, Colour{ Colours::Black } },
            FullScreenQuad::Vertex2D{ Vec2{ 1.0f, -1.0f}, Colour{ Colours::Black } },
            FullScreenQuad::Vertex2D{ Vec2{-1.0f, -1.0f}, Colour{ Colours::Black } },
            FullScreenQuad::Vertex2D{ Vec2{-1.0f,  1.0f}, Colour{ Colours::Black } }
        };
        const std::vector<uint32_t> mmQuadIndices = { 0, 1, 2, 2, 3, 0 };

        minimapQuad.vBuffer.Initialise(device, mmQuadVertices);
        minimapQuad.iBuffer.Initialise(device, mmQuadIndices);
    }


    void Engine::createDefaultMaterial()
    {
        #ifdef _DEBUG
            std::string defaultShaderPath = "build/Debug/";
        #else
            std::string defaultShaderPath = "build/Release/";
        #endif

        // Default Vertex Shader
        const std::vector<D3D11_INPUT_ELEMENT_DESC> VERTEX_LAYOUT_DESC =
        {
            {"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        MaterialSetup solidMaterial{};
        solidMaterial.VertexShaderPath          = defaultShaderPath + "VertexShader.cso";
        solidMaterial.VertexLayoutDescription   = VERTEX_LAYOUT_DESC;
        solidMaterial.PixelShaderPath           = defaultShaderPath + "PixelShader.cso";
        solidMaterial.VSCBufferSize             = sizeof(Renderer::DefaultVSCBuffer);
        solidMaterial.PSCBufferSize             = sizeof(Renderer::DefaultPSCBuffer);
        solidMaterial.FillMode                  = FillMode::Solid;
        solidMaterial.CullMode                  = CullMode::Back;

        materials.emplace(Graphics::StringHash("Solid"), Material{device, solidMaterial});
        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Material: \"Solid\"");
        #endif

        MaterialSetup wireframeMaterial = solidMaterial;
        wireframeMaterial.FillMode = FillMode::Wireframe;

        materials.emplace(Graphics::StringHash("Wireframe"), Material{device, wireframeMaterial});
        #ifdef _DEBUG
                Log(LogSeverity::Info, "Created a Material: \"Wireframe\"");
        #endif
    }

    void Engine::createTriangle()
    {
        const std::vector<Vertex> triangleVertices =
        {
            Vec3{ 0.0f,  0.5f,  0.0f},
            Vec3{ 0.5f, -0.5f,  0.0f},
            Vec3{-0.5f, -0.5f,  0.0f}
        };
        const std::vector<uint32_t> triangleIndices = { 0, 1, 2 };

        CreateModel("Triangle", triangleVertices, triangleIndices);
    }

    void Engine::createQuad()
    {
        const std::vector<Vertex> quadVertices =
        {
            Vec3{ 0.5f,  0.5f,  0.0f},
            Vec3{ 0.5f, -0.5f,  0.0f},
            Vec3{-0.5f, -0.5f,  0.0f},
            Vec3{-0.5f,  0.5f,  0.0f}
        };
        const std::vector<uint32_t> quadIndices = { 0, 1, 2, 2, 3, 0 };

        CreateModel("Quad", quadVertices, quadIndices);
    }

    void Engine::createSphere()
    {
        // Sphere
        //DirectX::GeometricPrimitive::VertexCollection   sphereVertexCollection;
        //DirectX::GeometricPrimitive::IndexCollection    sphereIndexCollection;
        //DirectX::GeometricPrimitive::CreateSphere(sphereVertexCollection, sphereIndexCollection);

        //std::vector<Vertex>     sphereVertices;
        //std::vector<uint32_t>   sphereIndices;
        //sphereIndices.resize(sphereIndexCollection.size());

        //for (const auto& V : sphereVertexCollection)
        //{
        //    sphereVertices.emplace_back();
        //    sphereVertices.back().Position = V.position;
        //}

        //for (size_t i = 0; i < sphereIndexCollection.size(); i += 3)
        //{
        //    // Swap vertex 2 and 3 for correct orientation
        //    sphereIndices[i]    = sphereIndexCollection[i];
        //    sphereIndices[i+1]  = sphereIndexCollection[i+2];
        //    sphereIndices[i+2]  = sphereIndexCollection[i+1];
        //}

        //CreateModel("Sphere", sphereVertices, sphereIndices);

        LoadModel("Sphere", "assets/Models/icosphere.obj");
    }

    void Engine::createCube()
    {
        // Cube
        //DirectX::GeometricPrimitive::VertexCollection   cubeVertexCollection;
        //DirectX::GeometricPrimitive::IndexCollection    cubeIndexCollection;
        //DirectX::GeometricPrimitive::CreateCube(cubeVertexCollection, cubeIndexCollection);

        //std::vector<Vertex>     cubeVertices;
        //std::vector<uint32_t>   cubeIndices;
        //cubeIndices.resize(cubeIndexCollection.size());


        //for (const auto& V : cubeVertexCollection)
        //{
        //    cubeVertices.emplace_back();
        //    cubeVertices.back().Position = V.position;
        //}

        //for (size_t i = 0; i < cubeIndexCollection.size(); i += 3)
        //{
        //    // Swap vertex 2 and 3 for correct orientation
        //    cubeIndices[i]    = cubeIndexCollection[i];
        //    cubeIndices[i+1]  = cubeIndexCollection[i+2];
        //    cubeIndices[i+2]  = cubeIndexCollection[i+1];
        //}

        //CreateModel("Cube", cubeVertices, cubeIndices);

        LoadModel("Cube", "assets/Models/cube.obj");
    }

    void Engine::updateMinimapCam()
    {
        const Vec3 NEW_MM_POS
        {
            defaultView.Camera->GetPosition().x
        ,   minimapView.Camera->GetPosition().y
        ,   defaultView.Camera->GetPosition().z
        };
        minimapView.Camera->SetPosition(NEW_MM_POS);

        const Vec3 NEW_MM_ROT { 0.0f, 0.0f, -defaultView.Camera->GetRotation().y };
        minimapView.Camera->SetRotation(NEW_MM_ROT);

        const float Y_POS     = minimapView.Camera->GetPosition().y;

        minimapView.Camera->SetOrthographic(-Y_POS, Y_POS, Y_POS, -Y_POS);
        minimapView.Camera->LookAt(Vec3{NEW_MM_POS.x, 0.0f, NEW_MM_POS.z});
    }


    bool Engine::resizeSwapchain()
    {
        // Release existing DirectX views and buffers
        if (depthStencilView)
        {
            depthStencilView->Release();
        }
        if (renderTargetView)
        {
            renderTargetView->Release();
        }

        HRESULT hresult = swapChain->ResizeBuffers
        (
            1, 
            static_cast<UINT>(window->GetWidth()), 
            static_cast<UINT>(window->GetHeight()), 
            DXGI_FORMAT_R8G8B8A8_UNORM, 
            0
        );
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to resize Swapchain!");
            return false;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        hresult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to get Swapchain's back buffer! Check if swapchain was successfully resized!");
            return false;
        }

        hresult = device.GetHandle()->CreateRenderTargetView
        (
            backBuffer.Get()
        ,   nullptr
        ,   renderTargetView.GetAddressOf()
        );
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create a Render Target!");
            return false;
        }

        D3D11_TEXTURE2D_DESC depthStencilDesc { 0 };
        depthStencilDesc.Width              = static_cast<UINT>(window->GetWidth());
        depthStencilDesc.Height             = static_cast<UINT>(window->GetHeight());
        depthStencilDesc.MipLevels          = 1U;
        depthStencilDesc.ArraySize          = 1U;
        depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count   = 1U;
        depthStencilDesc.SampleDesc.Quality = 0U;
        depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags     = 0U;
        depthStencilDesc.MiscFlags          = 0U;

        Graphics::Texture2D depthStencilBuffer;
        hresult = device.GetHandle()->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilBuffer.GetAddressOf());
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create depth stencil buffer.");
            return false;
        }

        hresult = device.GetHandle()->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, depthStencilView.GetAddressOf());
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create depth stencil view.");
            return false;
        }

        // Update Default Camera
        defaultView.Camera->SetPerspective
        (
            Math::Radians(80.0f),
            static_cast<float>(window->GetWidth()) / static_cast<float>(window->GetHeight()),
            0.1f,
            1000.0f
        );

        recreateViewports();
        return true;
    }

    void Engine::recreateViewports()
    {
        const float WINDOW_WIDTH    = static_cast<float>(window->GetWidth());
        const float WINDOW_HEIGHT   = static_cast<float>(window->GetHeight());

        defaultView.Viewport->SetWidth(WINDOW_WIDTH);
        defaultView.Viewport->SetHeight(WINDOW_HEIGHT);

        const Vec2 PADDING { 10.0f };

        minimapView.Viewport->SetTopLeftX(WINDOW_WIDTH - PADDING.x - minimapView.Viewport->GetWidth());
        minimapView.Viewport->SetTopLeftY(PADDING.y);
    }


    void Engine::renderFullScreenQuad()
    {
        fullscreenQuad.material->Set();

        const unsigned int STRIDE = sizeof(FullScreenQuad::Vertex2D);
        const unsigned int OFFSET = 0U;

        device.GetContext()->IASetVertexBuffers(0U, 1U, fullscreenQuad.vBuffer.GetBuffer().GetAddressOf(), &STRIDE, &OFFSET);
        device.GetContext()->IASetIndexBuffer(fullscreenQuad.iBuffer.GetBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        device.GetContext()->DrawIndexed(fullscreenQuad.iBuffer.GetIndexCount(), 0U, 0U);
    }

    void Engine::renderDefault()
    {
        // Render to default view
        defaultView.Viewport->Set(device);

        renderFullScreenQuad();

        if (defaultViewGrid)
            renderer->RenderGrid(defaultView);

        if (defaultViewAxes)
            renderer->RenderAxes(defaultView);

        renderer->Draw(device, *currentScene, defaultView);
    }


    void Engine::renderMinimap()
    {
        // Minimap view
        if (!minimapView.active)
            return;

        // Match minimap camera to default camera
        updateMinimapCam();

        minimapView.Viewport->Set(device);

        fullscreenQuad.material->Set();

        const unsigned int STRIDE = sizeof(FullScreenQuad::Vertex2D);
        const unsigned int OFFSET = 0U;

        device.GetContext()->IASetVertexBuffers(0U, 1U, minimapQuad.vBuffer.GetBuffer().GetAddressOf(), &STRIDE, &OFFSET);
        device.GetContext()->IASetIndexBuffer(minimapQuad.iBuffer.GetBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
        device.GetContext()->DrawIndexed(minimapQuad.iBuffer.GetIndexCount(), 0U, 0U);

        if (minimapAxes)
            renderer->RenderAxes(minimapView);

        renderer->Draw(device, *currentScene, minimapView);

        // Render Camera
        Vec3 p0 = defaultView.Camera->GetPosition();
        p0 += Vec3::Transform(Vec3{ 0.0f, 0.0f, 0.5f }, Mat4::CreateRotationY(-minimapView.Camera->GetRotation().z));
        Vec3 p1 = defaultView.Camera->GetPosition();
        p1 += Vec3::Transform(Vec3{ 0.5f, 0.0f,-0.5f }, Mat4::CreateRotationY(-minimapView.Camera->GetRotation().z));
        Vec3 p2 = defaultView.Camera->GetPosition();
        p2 += Vec3::Transform(Vec3{-0.5f, 0.0f,-0.5f }, Mat4::CreateRotationY(-minimapView.Camera->GetRotation().z));

        renderer->DrawTriangle(device, minimapView, p0, p1, p2, false, Colour{ Colours::Orange } );

        if (!minimapFOV)
            return;

        /// View Frustum
        const float HALF_FOV = defaultView.Camera->GetFieldOfView() * 0.5f;

        const Vec3 END = p0 + Vec3{ 0.0f, 0.0f, defaultView.Camera->GetFarPlane() };
        const Vec3 FOV_CCW  = Vec3::Transform(END - p0, Mat4::CreateRotationY(HALF_FOV - minimapView.Camera->GetRotation().z));
        const Vec3 FOV_CW   = Vec3::Transform(END - p0, Mat4::CreateRotationY(-HALF_FOV - minimapView.Camera->GetRotation().z));

        renderer->DrawLine(device, minimapView, p0, p0 + FOV_CCW, Colour{ Colours::Orange });
        renderer->DrawLine(device, minimapView, p0, p0 + FOV_CW, Colour{ Colours::Orange });
    }
}
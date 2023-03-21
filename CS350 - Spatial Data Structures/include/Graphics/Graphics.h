/************************************************************************************//*!
\file           Graphics.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of the lifecycle of the Graphics System.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Windows Headers
#include "wrl/client.h"
// DirectX Headers
#include <d3d11.h>
#include <PrimitiveBatch.h>
// Project Headers
#include "Math/MathDefines.h"
#include "Window/Window.h"
#include "Vertex.h"
#include "Viewport.h"
#include "Camera.h"
#include "Material.h"
#include "Model.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class Engine;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief    Encapsulates the Graphics System.
    *//*********************************************************************************/
    class Graphics
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using Device            = Microsoft::WRL::ComPtr<ID3D11Device>;
        using DeviceContext     = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;
        using RenderTargetView  = Microsoft::WRL::ComPtr<ID3D11RenderTargetView>;
        using Swapchain         = Microsoft::WRL::ComPtr<IDXGISwapChain>;
        using DepthStencilView  = Microsoft::WRL::ComPtr<ID3D11DepthStencilView>;
        using Texture2D         = Microsoft::WRL::ComPtr<ID3D11Texture2D>;

        using PrimitiveBatch    = std::unique_ptr<DirectX::PrimitiveBatch<Vertex>>;

        // Containers
        using Viewports         = std::unordered_map<size_t, Viewport>;
        using Materials         = std::unordered_map<size_t, Material>;
        using Models            = std::unordered_map<size_t, Model>;
        using Cameras           = std::unordered_map<size_t, Camera>;

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static std::hash<std::string>   StringHash;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] static Engine*   GetEngine()  { return engine; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static Engine& CreateEngine    (Window* window);
        static void    DestroyEngine   ();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        static Engine*                  engine;
    };
}
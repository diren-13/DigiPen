/************************************************************************************//*!
\file           Material.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a Material.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Windows Headers
#include <wrl/client.h>
// DirectX Headers
#include <d3d11.h>
// Project Headers
#include "Shader.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    enum class FillMode
    {
        Solid       = D3D11_FILL_SOLID
    ,   Wireframe   = D3D11_FILL_WIREFRAME
    };

    enum class PrimitiveOrientation
    {
        Clockwise
    ,   CounterClockwise
    };

    enum class CullMode
    {
        None    = D3D11_CULL_NONE
    ,   Front   = D3D11_CULL_FRONT
    ,   Back    = D3D11_CULL_BACK
    };

    enum class Topology
    {
        Points      = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
    ,   Lines       = D3D11_PRIMITIVE_TOPOLOGY_LINELIST
    ,   Triangles   = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    };

    struct MaterialSetup
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        std::string                             VertexShaderPath;
        std::vector<D3D11_INPUT_ELEMENT_DESC>   VertexLayoutDescription;
        std::string                             PixelShaderPath;
        size_t                                  VSCBufferSize   = 0U;
        size_t                                  PSCBufferSize   = 0U;
        CullMode                                CullMode        = CullMode::Back;
        FillMode                                FillMode        = FillMode::Solid;
        PrimitiveOrientation                    Orientation     = PrimitiveOrientation::Clockwise;
        bool                                    DepthTesting    = true;
    };

    /********************************************************************************//*!
    @brief    Encapsulates a Material for rendering.
    *//*********************************************************************************/
    class Material
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using RasteriserState   = Microsoft::WRL::ComPtr<ID3D11RasterizerState>;
        using DepthStencilState = Microsoft::WRL::ComPtr<ID3D11DepthStencilState>;
        

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Material (Device& device, const MaterialSetup& materialSetup);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const VertexShader&   GetVertexShader()   const   { return vertexShader; }
        [[nodiscard]] const PixelShader&    GetPixelShader()    const   { return pixelShader; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void Set                () const;
        void Set                (void* vsData, size_t vsNumBytes, void* psData, size_t psNumBytes);
        void SetVSCBufferData   (void* data, size_t numBytes);
        void SetPSCBufferData   (void* data, size_t numBytes);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Device&             device;

        VertexShader        vertexShader;
        PixelShader         pixelShader;
        RasteriserState     rasteriserState;
        DepthStencilState   depthStencilState;

        Topology            topology;
    };
}
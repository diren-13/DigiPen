/************************************************************************************//*!
\file           Material.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Implementation of a Material.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Material.h"
// Project Header
#include "Graphics/Graphics.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Material::Material(Device& _device, const MaterialSetup& setup)
    : device        { _device }
    , vertexShader  { setup.VertexShaderPath, setup.VertexLayoutDescription, setup.VSCBufferSize }
    , pixelShader   { setup.PixelShaderPath, setup.PSCBufferSize }
    , topology      { Topology::Triangles }
    {
        // Setup Rasteriser State
        D3D11_RASTERIZER_DESC rasteriserDesc;
        ZeroMemory(&rasteriserDesc, sizeof(D3D11_RASTERIZER_DESC));

        rasteriserDesc.FillMode                 = static_cast<D3D11_FILL_MODE>(setup.FillMode);
        rasteriserDesc.CullMode                 = static_cast<D3D11_CULL_MODE>(setup.CullMode);
        rasteriserDesc.FrontCounterClockwise    = (setup.Orientation == PrimitiveOrientation::CounterClockwise);
        HRESULT hresult = device.GetHandle()->CreateRasterizerState(&rasteriserDesc, rasteriserState.GetAddressOf());
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create Rasteriser State!");
            return;
        }

        // Setup Depth Stencil State
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc{ 0 };
        depthStencilDesc.DepthEnable    = setup.DepthTesting;
        depthStencilDesc.DepthWriteMask = setup.DepthTesting ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;

        hresult = device.GetHandle()->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create depth stencil state.");
            return;
        }

        vertexShader.Initialise(device);
        pixelShader.Initialise(device);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void Material::Set() const
    {;
        device.GetContext()->RSSetState(rasteriserState.Get());
        device.GetContext()->OMSetDepthStencilState(depthStencilState.Get(), 0);
        device.GetContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));

        // Set material
        device.GetContext()->IASetInputLayout(vertexShader.GetInputLayout());
        device.GetContext()->VSSetShader(vertexShader.GetHandle(), nullptr, 0);
        device.GetContext()->PSSetShader(pixelShader.GetHandle(), nullptr, 0);

        device.GetContext()->VSSetConstantBuffers(0, 1, vertexShader.cBuffer.GetBuffer().GetAddressOf());
        device.GetContext()->PSSetConstantBuffers(0, 1, pixelShader.cBuffer.GetBuffer().GetAddressOf());
    }

    void Material::Set(void* vsData, size_t vsNumBytes, void* psData, size_t psNumBytes)
    {
        SetVSCBufferData(vsData, vsNumBytes);
        SetPSCBufferData(psData, psNumBytes);
        Set();
    }

    void Material::SetVSCBufferData(void* data, size_t numBytes)
    {
        vertexShader.cBuffer.MapData(device, data, numBytes);
    }

    void Material::SetPSCBufferData(void* data, size_t numBytes)
    {
        pixelShader.cBuffer.MapData(device, data, numBytes);
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

}
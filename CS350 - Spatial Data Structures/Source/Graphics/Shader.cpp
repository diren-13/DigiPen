/************************************************************************************//*!
\file           Shader.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Implementation of the templated methods of the Shaders.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Shader.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Shader::Shader(const std::string& _shaderPath, size_t constantBufferSize)
    : shaderPath    { Utility::StringConverter::StringtoWString(_shaderPath) }
    , cBuffer       { constantBufferSize }
    {
        convertShaderToByteCode(shaderPath);
    }

    VertexShader::VertexShader(const std::string& _shaderPath, const LayoutDescription& _layoutDesc, size_t constantBufferSize)
    : Shader        { _shaderPath, constantBufferSize }
    {
        for (auto& desc : _layoutDesc)
        {
            layoutDesc.emplace_back(desc);
        }
    }

    PixelShader::PixelShader(const std::string& _shaderPath, size_t constantBufferSize)
    : Shader    { _shaderPath, constantBufferSize }
    {}


    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    void VertexShader::Initialise(Device& device)
    {
        const auto* BYTE_CODE       = this->shaderBuffer->GetBufferPointer();
        const auto  BYTE_CODE_SIZE  = this->shaderBuffer->GetBufferSize();

        HRESULT hresult = device.GetHandle()->CreateVertexShader(BYTE_CODE, BYTE_CODE_SIZE, nullptr, handle.GetAddressOf());
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create a Vertex Shader from: " + Utility::StringConverter::WStringtoString(this->shaderPath));
        }

        hresult = device.GetHandle()->CreateInputLayout
        (
            layoutDesc.data()
        ,   static_cast<UINT>(layoutDesc.size())
        ,   BYTE_CODE
        ,   BYTE_CODE_SIZE
        ,   inputLayout.GetAddressOf()
        );
        if (FAILED(hresult))
        {
            Log(LogSeverity::Error, "Failed to create an Input Layout!");
        }

        if (cBuffer.GetSize() > 0)
        {
            cBuffer.Initialise(device);
        }
    }


    void PixelShader::Initialise(Device& device)
    {
        const auto* BYTE_CODE       = this->shaderBuffer->GetBufferPointer();
        const auto  BYTE_CODE_SIZE  = this->shaderBuffer->GetBufferSize();

        const HRESULT HR = device.GetHandle()->CreatePixelShader(BYTE_CODE, BYTE_CODE_SIZE, nullptr, handle.GetAddressOf());
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to create a Pixel Shader: " + Utility::StringConverter::WStringtoString(this->shaderPath));
        }

        if (cBuffer.GetSize() > 0)
        {
            cBuffer.Initialise(device);
        }
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    void Shader::convertShaderToByteCode(const std::wstring& _shaderPath)
    {
        const HRESULT HR = D3DReadFileToBlob(_shaderPath.c_str(), shaderBuffer.GetAddressOf());
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to load shader: " + Utility::StringConverter::WStringtoString(_shaderPath));
        }
    }
}

/************************************************************************************//*!
\file           Shader.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a DirectX Shader.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <functional>
// Windows Headers
#include <wrl/client.h>
// DirectX Headers
#include <d3d11.h>
#include <d3dcompiler.h>
// Project Headers
#include "Tools/Converter.h"
#include "Device.h"
#include "Buffer.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declaration                                                             */
    /*---------------------------------------------------------------------------------*/

    class Material;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Shader
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] std::string                               GetShaderPath       ()  const   { return Utility::StringConverter::WStringtoString(shaderPath); }
        [[nodiscard]] const Microsoft::WRL::ComPtr<ID3D10Blob>& GetBuffer           ()  const   { return shaderBuffer.Get(); }
        [[nodiscard]] const ConstantBuffer&                     GetConstantBuffer   ()  const   { return cBuffer; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        virtual void Initialise (Device& device) = 0;

    protected:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Shader  (const std::string& shaderPath, size_t constantBufferSize = 0U);

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        std::wstring                                shaderPath;
        Microsoft::WRL::ComPtr<ID3D10Blob>          shaderBuffer;   // Holds the byte-code of the shader
        ConstantBuffer                              cBuffer;

        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class Material;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void convertShaderToByteCode    (const std::wstring& shaderPath);

        
    };

    class VertexShader : public Shader
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using InputLayout       = Microsoft::WRL::ComPtr<ID3D11InputLayout>;
        using LayoutDescription = std::vector<D3D11_INPUT_ELEMENT_DESC>;

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        VertexShader    (const std::string& shaderPath, const LayoutDescription& layoutDesc, size_t constantBufferSize = 0U);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] ID3D11VertexShader*       GetHandle()             const   { return handle.Get(); }
        [[nodiscard]] ID3D11InputLayout*        GetInputLayout()        const   { return inputLayout.Get(); }
        [[nodiscard]] const LayoutDescription&  GetLayoutDescription()  const   { return layoutDesc; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void Initialise(Device& device) override;

    protected:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Microsoft::WRL::ComPtr<ID3D11VertexShader>  handle;
        InputLayout                                 inputLayout;

        LayoutDescription                           layoutDesc;
    };

    class PixelShader : public Shader
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        PixelShader    (const std::string& shaderPath, size_t constantBufferSize = 0U);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] ID3D11PixelShader* GetHandle() const   { return handle.Get(); }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void Initialise(Device& device) override;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Microsoft::WRL::ComPtr<ID3D11PixelShader>  handle;
    };
}

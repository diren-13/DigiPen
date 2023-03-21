/************************************************************************************//*!
\file           Buffer.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Implementation for a Buffer.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Buffer.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Buffer::Buffer(Type _type)
    : type  { _type }
    {}

    ConstantBuffer::ConstantBuffer(size_t _size)
    : Buffer { Type::Constant }
    , size   { _size }
    {}

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    bool ConstantBuffer::Initialise(Device& device)
    {
        if (size % 16 != 0)
        {
            Log(LogSeverity::Error, "Size of constant buffer is not aligned to 16 bytes!");
            return false;
        }

        D3D11_BUFFER_DESC constantBufferDesc{ 0 };

        constantBufferDesc.Usage            = D3D11_USAGE_DYNAMIC;
        constantBufferDesc.ByteWidth        = static_cast<UINT>(size);      // Align to 16-bytes 
        constantBufferDesc.BindFlags        = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
        constantBufferDesc.MiscFlags        = 0;

        const HRESULT HR = device.GetHandle()->CreateBuffer(&constantBufferDesc, nullptr, buffer.GetAddressOf());
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to create an Constant Buffer!");
            return false;
        }

        return true;
    }

    void ConstantBuffer::MapData(Device& device, void* data, size_t dataSize) const
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        const HRESULT HR = device.GetContext()->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to map constant buffer.");
            return;
        }

        CopyMemory(mappedResource.pData, reinterpret_cast<char*>(data), dataSize);
        device.GetContext()->Unmap(buffer.Get(), 0);
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

}
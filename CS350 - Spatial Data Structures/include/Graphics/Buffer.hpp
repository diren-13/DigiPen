/************************************************************************************//*!
\file           Buffer.hpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Implementation for templated Buffers.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Buffer.h"
// Project Headers
#include "Tools/Console.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    template <typename T>
    VertexBuffer<T>::VertexBuffer()
    : Buffer        { Type::Vertex }
    , vertexCount   { 0U }
    {}

    template <typename T>
    VertexBuffer<T>::VertexBuffer(const std::vector<T>& _vertices)
    : Buffer        { Type::Vertex }
    , vertexCount   { static_cast<unsigned int>(_vertices.size()) }
    {
        vertices.resize(vertexCount);
        std::copy(_vertices.cbegin(), _vertices.cend(), vertices.begin());
    }

    template <typename T>
    VertexBuffer<T>::VertexBuffer(const T* _vertices, unsigned int numVertices)
    : Buffer        { Type::Vertex }
    , vertexCount   { numVertices }
    {
        vertices.resize(vertexCount);
        std::copy_n(_vertices, numVertices, vertices.begin());
    }

    template <typename T>
    IndexBuffer<T>::IndexBuffer()
    : Buffer        { Type::Index }
    , indexCount    { 0 }
    {}

    template <typename T>
    IndexBuffer<T>::IndexBuffer(const std::vector<T>& _indices)
    : Buffer        { Type::Index }
    , indexCount    { static_cast<unsigned int>(_indices.size()) }
    {
        indices.resize(indexCount);
        std::copy(_indices.begin(), _indices.end(), indices.begin());
    }

    template <typename T>
    IndexBuffer<T>::IndexBuffer(const T* _indices, unsigned int numIndices)
    : Buffer        { Type::Index }
    , indexCount    { numIndices }
    {
        indices.resize(indexCount);
        std::copy_n(_indices, numIndices, indices.begin());
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    template <typename T>
    bool VertexBuffer<T>::Initialise(Device& device)
    {
        D3D11_BUFFER_DESC vertexBufferDesc{ 0 };

        vertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth      = sizeof(T) * vertexCount;
        vertexBufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags      = 0;

        D3D11_SUBRESOURCE_DATA vertexBufferData{ nullptr };
        vertexBufferData.pSysMem = vertices.data();

        const HRESULT HR = device.GetHandle()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, buffer.GetAddressOf());
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to create a Vertex Buffer!");
            return false;
        }

        return true;
    }

    template <typename T>
    bool VertexBuffer<T>::Initialise(Device& device, const std::vector<T>& _vertices)
    {
        vertices.resize(_vertices.size());
        std::copy(_vertices.cbegin(), _vertices.cend(), vertices.begin());
        vertexCount = static_cast<unsigned int>(vertices.size());

        return Initialise(device);
    }

    template <typename T>
    bool VertexBuffer<T>::Initialise(Device& device, const T* _vertices, unsigned int numVertices)
    {
        vertices.resize(numVertices);
        std::copy_n(_vertices, numVertices, vertices.begin());
        vertexCount = numVertices;

        return Initialise(device);
    }

    template <typename T>
    bool IndexBuffer<T>::Initialise(Device& device)
    {
        D3D11_BUFFER_DESC indexBufferDesc{ 0 };

        indexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth      = sizeof(T) * indexCount;
        indexBufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags      = 0;

        D3D11_SUBRESOURCE_DATA indexBufferData{ 0 };
        indexBufferData.pSysMem = indices.data();

        const HRESULT HR = device.GetHandle()->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer.GetAddressOf());
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to create an Index Buffer!");
            return false;
        }

        return true;
    }

    template <typename T>
    bool IndexBuffer<T>::Initialise(Device& device, const std::vector<T>& _indices)
    {
        indices.resize(_indices.size());
        std::copy(_indices.cbegin(), _indices.cend(), indices.begin());
        indexCount = static_cast<unsigned int>(indices.size());

        return Initialise(device);
    }

    template <typename T>
    bool IndexBuffer<T>::Initialise(Device& device, const T* _indices, unsigned int numIndices)
    {
        indices.resize(numIndices);
        std::copy_n(_indices, numIndices, indices.begin());
        indexCount = numIndices;

        return Initialise(device);
    }
}

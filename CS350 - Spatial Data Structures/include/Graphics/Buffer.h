/************************************************************************************//*!
\file           Buffer.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a Buffer, Vertex Buffer, Index Buffer and a Buffer Factory.
 
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
#include "Device.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief      Encapsulates a DirectX Buffer.
    *//*********************************************************************************/
    class Buffer
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using NativeBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;

        /****************************************************************************//*!
        @brief  The Buffer's Type.
        *//*****************************************************************************/
        enum class Type
        {
            Vertex
        ,   Index
        ,   Constant
        };

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] NativeBuffer  GetBuffer() const   { return buffer; }
        [[nodiscard]] Type          GetType()   const   { return type; }

    protected:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Buffer  (Type type);

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        NativeBuffer    buffer;
        Type            type;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class BufferFactory;
    };

    template <typename T>
    class VertexBuffer : public Buffer
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        VertexBuffer    ();
        VertexBuffer    (const std::vector<T>& vertices);
        VertexBuffer    (const T* vertices, unsigned int numVertices);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const std::vector<T>& GetVertices     ()  const   { return vertices; }
        [[nodiscard]] unsigned int          GetVertexCount  ()  const   { return vertexCount; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        bool Initialise(Device& device);
        bool Initialise(Device& device, const std::vector<T>& vertices);
        bool Initialise(Device& device, const T* vertices, unsigned int numVertices);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        std::vector<T>  vertices;
        unsigned int    vertexCount;
    };

    template <typename T>
    class IndexBuffer : public Buffer
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        IndexBuffer ();
        IndexBuffer (const std::vector<T>& indices);
        IndexBuffer (const T* indices, unsigned int numIndices);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const std::vector<T>& GetIndices      ()  const   { return indices; }
        [[nodiscard]] unsigned int          GetIndexCount   ()  const   { return indexCount; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        bool Initialise(Device& device);
        bool Initialise(Device& device, const std::vector<T>& indices);
        bool Initialise(Device& device, const T* indices, unsigned int numIndices);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        std::vector<T>  indices;
        unsigned int    indexCount;
    };

    class ConstantBuffer : public Buffer
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        ConstantBuffer (size_t size);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] size_t GetSize() const { return size; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        bool Initialise (Device& device);
        void MapData    (Device& device, void* data, size_t dataSize) const;

    private:
        size_t size;
    };
}

#include "Buffer.hpp"
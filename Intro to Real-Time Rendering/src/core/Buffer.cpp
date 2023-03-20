/*+======================================================================================
File:       Buffer.cpp

Date:       8th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for a Buffer

Functions:  RTR::Buffer::Buffer
            RTR::Buffer::~Buffer

            RTR::Buffer::MapMemory
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core/Buffer.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    Buffer::Buffer(Device& _device, Type _type, uint32_t _size, uint32_t _count)
    : device    { &_device }
    , type      { _type }
    , size      { _size }
    , count     { _count }
    {
        vk::BufferCreateInfo bufferCreateInfo =
        {
            .size           = static_cast<vk::DeviceSize>(size) * count,
            .usage          = vk::BufferUsageFlagBits::eTransferSrc,
            .sharingMode    = vk::SharingMode::eExclusive
        };

        switch (type)
        {
            case Type::Vertex:  bufferCreateInfo.usage  |= vk::BufferUsageFlagBits::eVertexBuffer;   break;
            case Type::Index:   bufferCreateInfo.usage  |= vk::BufferUsageFlagBits::eIndexBuffer;    break;
            case Type::Staging: break;

            default:
                throw std::runtime_error("[Buffer::Buffer] Cannot initialise a buffer with an invalid type!");
        }

        try
        {
            handle = device->GetHandle().createBuffer(bufferCreateInfo);
            Utility::LogMessage("Created a buffer!");

            // Get memory requirements for the buffer
            vk::MemoryRequirements memoryRequirements;
            device->GetHandle().getBufferMemoryRequirements(handle, &memoryRequirements);

            vk::MemoryPropertyFlags flags = vk::MemoryPropertyFlagBits::eHostVisible;
            flags |= vk::MemoryPropertyFlagBits::eHostCoherent;

            const vk::MemoryAllocateInfo BUFFER_MEM_ALLOC_INFO =
            {
                .allocationSize     = memoryRequirements.size,
                .memoryTypeIndex    = device->GetGPU().FindMemoryType(memoryRequirements.memoryTypeBits, flags)
            };

            deviceMemory = device->GetHandle().allocateMemory(BUFFER_MEM_ALLOC_INFO);
            device->GetHandle().bindBufferMemory(handle, deviceMemory, 0);
        }
        catch(const vk::SystemError&)
        {
            throw std::runtime_error("[Buffer::Buffer] Failed to create & allocate a buffer!");
        }
    }

    Buffer::~Buffer()
    {
        device->GetHandle().destroyBuffer(handle);
        device->GetHandle().freeMemory(deviceMemory);

        Utility::LogMessage("Destroyed a buffer!");
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/
    void Buffer::MapMemory(const void* data, size_t offset)
    {
        if (!data)
            throw std::runtime_error("[Buffer::MapMemory] No data passed in to map to the buffer!");

        size_t BUFFER_SIZE = static_cast<size_t>(size) * count;

        const vk::MemoryMapFlags EMPTY_FLAG;
        void* mem = device->GetHandle().mapMemory(deviceMemory, offset, BUFFER_SIZE, EMPTY_FLAG);
        memcpy_s(mem, BUFFER_SIZE, data, BUFFER_SIZE);
        device->GetHandle().unmapMemory(deviceMemory);

        if (size == 0)
        {
            Utility::LogWarning("Buffer elements are empty! Cannot Map an Empty Buffer!");
            return;
        }
        if (count == 0)
        {
            Utility::LogWarning("Buffer has no elements! Cannot Map an Empty Buffer!");
            return;
        }

        vk::BufferCreateInfo finalBufferCreateInfo = 
        {
            .size          = static_cast<uint32_t>(BUFFER_SIZE),
            .usage         = vk::BufferUsageFlagBits::eTransferDst,
            .sharingMode   = vk::SharingMode::eExclusive
        };

        switch (type)
        {
            case Type::Vertex:  finalBufferCreateInfo.usage |= vk::BufferUsageFlagBits::eVertexBuffer;   break;
            case Type::Index:   finalBufferCreateInfo.usage |= vk::BufferUsageFlagBits::eIndexBuffer;    break;
            case Type::Staging: break;

            default:
                throw std::runtime_error("[Buffer::Buffer] Cannot initialise a buffer with an invalid type!");
        }

        vk::Buffer          finalHandle;
        vk::DeviceMemory    finalDeviceMemory;

        try
        {
            finalHandle = device->GetHandle().createBuffer(finalBufferCreateInfo);

            // Get memory requirements for the buffer
            vk::MemoryRequirements finalMemoryRequirements;
            device->GetHandle().getBufferMemoryRequirements(finalHandle, &finalMemoryRequirements);

            const vk::MemoryAllocateInfo FINAL_BUFFER_MEM_ALLOC_INFO =
            {
                .allocationSize     = finalMemoryRequirements.size,
                .memoryTypeIndex    = device->GetGPU().FindMemoryType(finalMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
            };

            finalDeviceMemory = device->GetHandle().allocateMemory(FINAL_BUFFER_MEM_ALLOC_INFO);
            device->GetHandle().bindBufferMemory(finalHandle, finalDeviceMemory, 0);
        }
        catch(const vk::SystemError&)
        {
            throw std::runtime_error("[Buffer::MapMemory] Failed to create & allocate a buffer!");
        }

        // Copy data from staging buffer to local GPU memory
        device->BeginSingleTimeCommand();
        device->CopyBuffer(handle, finalHandle, BUFFER_SIZE);
        device->EndSingleTimeCommand();

        device->GetHandle().destroyBuffer(handle);
        device->GetHandle().freeMemory(deviceMemory);

        handle          = finalHandle;
        deviceMemory    = finalDeviceMemory;
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/


} // namespace RTR
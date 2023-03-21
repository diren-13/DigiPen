/*+======================================================================================
File:       Buffer.h

Date:       8th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Buffer

Classes:    RTR::Buffer
======================================================================================+*/

#pragma once

// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Device.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class BufferFactory;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a generic Buffer object for Vulkan.
    /// </summary>
    class Buffer
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Represents the type of the buffer
        /// </summary>
        enum class Type
        {
            Vertex,
            Index,
            Staging
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructors                                                  */
        /*-----------------------------------------------------------------------------*/
        Buffer  (Device& _device, Type type, uint32_t size, uint32_t count);
        ~Buffer ();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline vk::Buffer         GetHandle() const   { return handle; }
        [[nodiscard]] inline vk::DeviceMemory   GetMemory() const   { return deviceMemory; }
        [[nodiscard]] inline const Type         GetType()   const   { return type; }
        [[nodiscard]] inline const uint32_t     GetSize()   const   { return size; }
        [[nodiscard]] inline const uint32_t     GetCount()  const   { return count; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Maps data to the buffer.
        /// </summary>
        /// <param name="data">
        /// The data to map.
        /// </param>
        /// <param name="offset">
        /// The offset of the data to map. Defaults to 0.
        /// </param>
        void MapMemory      (const void* data, size_t offset = 0);

    private:
        

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Buffer          handle;
        vk::DeviceMemory    deviceMemory;

        Device*             device;

        Type                type;
        uint32_t            size;
        uint32_t            count;
    };
    
} // namespace RTR
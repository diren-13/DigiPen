/*+======================================================================================
File:       TextureSampler.h

Date:       18th April 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a TextureSampler.

Classes:    RTR::TextureSampler
======================================================================================+*/

#pragma once

// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Device.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Texture Sampler.
    /// </summary>
    class TextureSampler
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        // TODO: Enum wrappers around filter types
        // TODO: Enum wrappers around modes
        // TODO: Setup struct


        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        TextureSampler();
        TextureSampler(Device& _device);
        TextureSampler(TextureSampler&& rhs);
        TextureSampler& operator=(TextureSampler&& rhs);
        ~TextureSampler();

        TextureSampler(const TextureSampler&)               = delete;
        TextureSampler& operator=(const TextureSampler&)    = delete;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] vk::Sampler   GetHandle() const   { return handle; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Sampler handle;

        Device*     device;

        
    };


} // namespace RTR

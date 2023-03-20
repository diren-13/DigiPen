/*+======================================================================================
File:       Depthbuffer.h

Date:       10th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Depthbuffer.

Classes:    RTR::Depthbuffer
======================================================================================+*/

#pragma once

// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Device.h"
#include "SwapChain.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Depthbuffer
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Depthbuffer() = default;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::Image&       GetImage()      const   { return depthImage; }
        [[nodiscard]] inline const vk::ImageView&   GetImageView()  const   { return depthImageView; }


        /*-----------------------------------------------------------------------------*/
        /* Function Memebers                                                           */
        /*-----------------------------------------------------------------------------*/
        void                Initalise
        (
            Device&                 device, 
            SwapChain&              swapChain, 
            vk::ImageTiling         tiling      = vk::ImageTiling::eOptimal, 
            vk::FormatFeatureFlags  features    = vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );

        static vk::Format   FindDepthFormat
        (
            Device&                 device,
            vk::ImageTiling         tiling      = vk::ImageTiling::eOptimal, 
            vk::FormatFeatureFlags  features    = vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );

        void                Destroy         (Device& device);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Image           depthImage;
        vk::ImageView       depthImageView;
        vk::DeviceMemory    depthImageMemory;
    };


} // namespace RTR
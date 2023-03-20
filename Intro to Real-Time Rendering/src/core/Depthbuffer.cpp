/*+======================================================================================
File:       Depthbuffer.cpp

Date:       10th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the Depthbuffer Class.

Functions:  RTR::Depthbuffer::Depthbuffer
            RTR::Depthbuffer::~Depthbuffer

            
======================================================================================+*/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "core/Depthbuffer.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Public Member Functions                                                         */
    /*---------------------------------------------------------------------------------*/
    void Depthbuffer::Initalise
    (
        Device&                 device, 
        SwapChain&              swapChain, 
        vk::ImageTiling         tiling, 
        vk::FormatFeatureFlags  features
    )
    {
        vk::Format depthFormat = FindDepthFormat(device, tiling, features);

        vk::ImageCreateInfo imageInfo =
        {
            .imageType      = vk::ImageType::e2D,
            .format         = depthFormat,
            .mipLevels      = 1,
            .arrayLayers    = 1,
            .samples        = vk::SampleCountFlagBits::e1,
            .tiling         = tiling,
            .usage          = vk::ImageUsageFlagBits::eDepthStencilAttachment,
            .sharingMode    = vk::SharingMode::eExclusive,
            .initialLayout  = vk::ImageLayout::eUndefined
        };

        imageInfo.extent.width  = swapChain.GetProperties().Extent.width;
        imageInfo.extent.height = swapChain.GetProperties().Extent.height;
        imageInfo.extent.depth  = 1U;

        try
        {
            depthImage = device.GetHandle().createImage(imageInfo);

            std::optional<uint32_t> memoryTypeIndex;

            vk::MemoryRequirements memoryRequirements;
            device.GetHandle().getImageMemoryRequirements(depthImage, &memoryRequirements);

            vk::PhysicalDeviceMemoryProperties gpuMemoryProperties;
            device.GetGPUHandle().getMemoryProperties(&gpuMemoryProperties);

            for (uint32_t i = 0; i < gpuMemoryProperties.memoryTypeCount; ++i) 
            {
                if
                (
                    (memoryRequirements.memoryTypeBits & (1 << i)) && 
                    (gpuMemoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) 
                    == vk::MemoryPropertyFlagBits::eDeviceLocal
                ) 
                {
                    memoryTypeIndex = i;
                    break;
                }
            }

            if (!memoryTypeIndex.has_value())
                throw std::runtime_error("[Depthbuffer::Initalise] Failed to find a suitable memory type!");

            vk::MemoryAllocateInfo memoryAllocateInfo =
            {
                .allocationSize     = memoryRequirements.size,
                .memoryTypeIndex    = memoryTypeIndex.value()
            };

            depthImageMemory = device.GetHandle().allocateMemory(memoryAllocateInfo);
            device.GetHandle().bindImageMemory(depthImage, depthImageMemory, 0);

            vk::ImageViewCreateInfo viewInfo =
            {
                .image      = depthImage,
                .viewType   = vk::ImageViewType::e2D,
                .format     = depthFormat
            };

            viewInfo.subresourceRange.aspectMask        = vk::ImageAspectFlagBits::eDepth;
            viewInfo.subresourceRange.baseMipLevel      = 0;
            viewInfo.subresourceRange.levelCount        = 1;
            viewInfo.subresourceRange.baseArrayLayer    = 0;
            viewInfo.subresourceRange.layerCount        = 1;

            depthImageView = device.GetHandle().createImageView(viewInfo);

        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Depthbuffer::Initalise] Failed to create a depth buffer.");
        }
    }

    vk::Format Depthbuffer::FindDepthFormat
    (
        Device&                 device,
        vk::ImageTiling         tiling, 
        vk::FormatFeatureFlags  features
    )
    {
        std::optional<vk::Format> depthFormat;

        std::vector candidates =
        {
            vk::Format::eD32Sfloat,
            vk::Format::eD32SfloatS8Uint,
            vk::Format::eD24UnormS8Uint
        };

        for (auto format : candidates)
        {
            vk::FormatProperties properties;
            device.GetGPUHandle().getFormatProperties(format, &properties);

            const bool LINEAR_CHECK = (tiling == vk::ImageTiling::eLinear) && (properties.linearTilingFeatures & features) == features;
            const bool OPTIMAL_CHECK = (tiling == vk::ImageTiling::eOptimal) && (properties.optimalTilingFeatures & features) == features;

            if (LINEAR_CHECK || OPTIMAL_CHECK)
            {
                depthFormat = format;
                break;
            }
        }

        if (!depthFormat.has_value())
                throw std::runtime_error("[Depthbuffer::Initalise] Failed to find a suitable format!");

        return depthFormat.value();
    }


    void Depthbuffer::Destroy(Device& device)
    {
        device.GetHandle().destroyImageView(depthImageView);
        device.GetHandle().destroyImage(depthImage);

        device.GetHandle().freeMemory(depthImageMemory);
    }


} // namespace RTR
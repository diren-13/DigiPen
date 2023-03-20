/*+======================================================================================
File:       SwapChain.cpp

Date:       16th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the SwapChain class.

Functions:  RTR::SwapChainProperties::SwapChainProperties
            RTR::SwapChain::SwapChain

            RTR::SwapChain::AttachInstance
            RTR::SwapChain::AttachDevice
            RTR::SwapChain::AttachWindow
            RTR::SwapChain::Initialise
            RTR::SwapChain::Recreate
            RTR::SwapChain::Destroy

            RTR::SwapChain::createSwapChain
            RTR::SwapChain::createImageViews
            RTR::SwapChain::chooseSurfaceFormat
            RTR::SwapChain::choosePresentMode
            RTR::SwapChain::chooseExtent
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Standard Libraries
#include <numeric>
// Primary Header
#include "core/SwapChain.h"


namespace RTR
{
    SwapChainProperties::SwapChainProperties()
    : PresentMode {}
    {}


    SwapChain::SwapChain()
    : hasSurface    { false }
    {}

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void SwapChain::CreateSurface(Instance& instance, Window& window)
    {
        try
        {
            // Create Surface
            const vk::Win32SurfaceCreateInfoKHR SURFACE_CREATE_INFO =
            {
                .hinstance  = GetModuleHandle(nullptr),
                .hwnd       = window.GetWindow() 
            };

            surface = instance.GetHandle().createWin32SurfaceKHR(SURFACE_CREATE_INFO);
            Utility::LogMessage("Created a Surface!");

            hasSurface = true;
        }
        catch (const vk::SystemError)
        {
            throw std::runtime_error("[SwapChain::CreateSurface] Failed to create a surface!");
        }
    }


    void SwapChain::Initialise(Device& device, Window& window)
    {
        if (!hasSurface)
            throw std::runtime_error("[SwapChain::Initialise] Cannot initialise without a surface!");

        // Get properties
        vk::SurfaceCapabilitiesKHR capabilities = device.GetGPUHandle().getSurfaceCapabilitiesKHR(surface);

        std::vector<vk::SurfaceFormatKHR> formats;
        uint32_t formatCount = 0;
        device.GetGPUHandle().getSurfaceFormatsKHR(surface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            formats.resize(formatCount);
            device.GetGPUHandle().getSurfaceFormatsKHR(surface, &formatCount, formats.data());
        }

        std::vector<vk::PresentModeKHR> presentModes;
        uint32_t presentModeCount = 0;
        device.GetGPUHandle().getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            presentModes.resize(presentModeCount);
            device.GetGPUHandle().getSurfacePresentModesKHR(surface, &presentModeCount, presentModes.data());
        }

        // Check if device is compatible with swapchain
        const bool SURFACE_SUPPORT = device.IsSurfaceSupported(surface);
        const bool FORMATS_SUPPORT = !formats.empty();
        const bool PRESENT_SUPPORT = !presentModes.empty();

        if (!SURFACE_SUPPORT || !FORMATS_SUPPORT || !PRESENT_SUPPORT)
            throw std::runtime_error("[SwapChain] Device cannot support current surface!");

        // Select the correct properties
        properties.Extent       = chooseExtent(capabilities, window.GetWindowWidth(), window.GetWindowHeight());
        properties.Format       = chooseSurfaceFormat(formats);
        properties.PresentMode  = choosePresentMode(presentModes);

        // Create Swap Chain
        createSwapChain(device, capabilities);
        createImageViews(device);
    }

    void SwapChain::Initialise(vk::SwapchainKHR oldSwapchain, Device& device, Window& window)
    {
        vk::SwapchainKHR oldSwapChain = handle;

        vk::SurfaceCapabilitiesKHR capabilities = device.GetGPUHandle().getSurfaceCapabilitiesKHR(surface);

        std::vector<vk::SurfaceFormatKHR> formats;
        uint32_t formatCount = 0;
        device.GetGPUHandle().getSurfaceFormatsKHR(surface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            formats.resize(formatCount);
            device.GetGPUHandle().getSurfaceFormatsKHR(surface, &formatCount, formats.data());
        }

        std::vector<vk::PresentModeKHR> presentModes;
        uint32_t presentModeCount = 0;
        device.GetGPUHandle().getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            presentModes.resize(presentModeCount);
            device.GetGPUHandle().getSurfacePresentModesKHR(surface, &presentModeCount, presentModes.data());
        }

        const bool SURFACE_SUPPORT = device.IsSurfaceSupported(surface);
        const bool FORMATS_SUPPORT = !formats.empty();
        const bool PRESENT_SUPPORT = !presentModes.empty();

        if (!SURFACE_SUPPORT || !FORMATS_SUPPORT || !PRESENT_SUPPORT)
            throw std::runtime_error("[SwapChain] Device cannot support current surface!");

        properties.Extent       = chooseExtent(capabilities, window.GetWindowWidth(), window.GetWindowHeight());
        properties.Format       = chooseSurfaceFormat(formats);
        properties.PresentMode  = vk::PresentModeKHR::eFifo;        // Set FIFO for forced Vsync

        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        const uint32_t QUEUE_FAMILY_INDICES[2] =
        {
            device.GetTransferQueueIndex(),
            device.GetMainQueueIndex()
        };

        const bool ARE_INDICES_SAME = (device.GetMainQueueIndex() == device.GetTransferQueueIndex());

        const vk::SwapchainCreateInfoKHR SWAPCHAIN_CREATE_INFO =
        {
            .surface                = surface,
            .minImageCount          = imageCount,
            .imageFormat            = properties.Format.format,
            .imageColorSpace        = properties.Format.colorSpace,
            .imageExtent            = properties.Extent,
            .imageArrayLayers       = 1,
            .imageUsage             = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode       = ARE_INDICES_SAME ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
            .queueFamilyIndexCount  = static_cast<uint32_t>(ARE_INDICES_SAME ? 0 : 2),
            .pQueueFamilyIndices    = ARE_INDICES_SAME ? nullptr : QUEUE_FAMILY_INDICES,
            .preTransform           = capabilities.currentTransform,
            .compositeAlpha         = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode            = properties.PresentMode,
            .clipped                = true,
            .oldSwapchain           = oldSwapchain 
        };

        try
        {
            handle = device.GetHandle().createSwapchainKHR(SWAPCHAIN_CREATE_INFO);
            Utility::LogMessage("Created a Swapchain!");
            device.GetHandle().destroySwapchainKHR(oldSwapchain);

            device.GetHandle().getSwapchainImagesKHR(handle, &imageCount, nullptr);
            images.resize(imageCount);
            device.GetHandle().getSwapchainImagesKHR(handle, &imageCount, images.data());
        }
        catch(vk::SystemError error)
        {
            throw std::runtime_error("[SwapChain::Initialise] Created a Swapchain!");
        }

        imageViews.resize(images.size());

        // Create an image view for each image
        for (size_t i = 0; i < images.size(); ++i)
        {
            vk::ImageViewCreateInfo imageViewCreateInfo =
            {
                .image          = images[i],
                .viewType       = vk::ImageViewType::e2D,
                .format         = properties.Format.format
            };

            imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
            imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
            imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
            imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;

            imageViewCreateInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
            imageViewCreateInfo.subresourceRange.levelCount     = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            // Multiple layers are used for stereoscopic 3D
            imageViewCreateInfo.subresourceRange.layerCount     = 1;

            imageViews[i] = device.GetHandle().createImageView(imageViewCreateInfo);
        }
    }

    void SwapChain::Recreate(Device& device, Window& window)
    {
        for (auto& view : imageViews)
        {
            device.GetHandle().destroyImageView(view);
        }

        Initialise(handle, device, window);
    }


    void SwapChain::Destroy(Instance& instance, Device& device) const
    {
        Utility::LogMessage("Destroying Swapchain...");
        device.GetHandle().destroySwapchainKHR(handle);

        Utility::LogMessage("Destroying Image Views...");
        for (auto& view : imageViews)
        {
            device.GetHandle().destroyImageView(view);
        }
        Utility::LogMessage("Destroyed Image Views!");

        instance.GetHandle().destroySurfaceKHR(surface);
        Utility::LogMessage("Destroyed a Surface!");
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/

    void SwapChain::createSwapChain(Device& device, const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        const uint32_t QUEUE_FAMILY_INDICES[2] =
        {
            device.GetTransferQueueIndex(),
            device.GetMainQueueIndex()
        };

        const bool ARE_INDICES_SAME = (device.GetMainQueueIndex() == device.GetTransferQueueIndex());

        const vk::SwapchainCreateInfoKHR SWAPCHAIN_CREATE_INFO =
        {
            .surface                = surface,
            .minImageCount          = imageCount,
            .imageFormat            = properties.Format.format,
            .imageColorSpace        = properties.Format.colorSpace,
            .imageExtent            = properties.Extent,
            // Always 1 unless application is stereoscopic 3D
            .imageArrayLayers       = 1,
            // Method to render to the image in the swap chain. Use eTransferDst for deferred rendering.
            .imageUsage             = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode       = ARE_INDICES_SAME ? vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
            .queueFamilyIndexCount  = static_cast<uint32_t>(ARE_INDICES_SAME ? 0 : 2),
            .pQueueFamilyIndices    = ARE_INDICES_SAME ? nullptr : QUEUE_FAMILY_INDICES,
            // Specify special transformation. Use current transform to ignore.
            .preTransform           = capabilities.currentTransform,
            // Alpha channel for blending with other windows. Opaque ignores any blending.
            .compositeAlpha         = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode            = properties.PresentMode,
            .clipped                = true,
        };

        try
        {
            handle = device.GetHandle().createSwapchainKHR(SWAPCHAIN_CREATE_INFO);
            Utility::LogMessage("Created a Swapchain!");

            device.GetHandle().getSwapchainImagesKHR(handle, &imageCount, nullptr);
            images.resize(imageCount);
            device.GetHandle().getSwapchainImagesKHR(handle, &imageCount, images.data());
        }
        catch(const vk::SystemError&)
        {
            throw std::runtime_error("[SwapChain::Initialise] Created a Swapchain!");
        }
    }

    void SwapChain::createImageViews(Device& device)
    {
        imageViews.resize(images.size());

        // Create an image view for each image
        for (size_t i = 0; i < images.size(); ++i)
        {
            vk::ImageViewCreateInfo imageViewCreateInfo =
            {
                .image          = images[i],
                .viewType       = vk::ImageViewType::e2D,
                .format         = properties.Format.format
            };

            imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
            imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
            imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
            imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;

            imageViewCreateInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
            imageViewCreateInfo.subresourceRange.levelCount     = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            // Multiple layers are used for stereoscopic 3D
            imageViewCreateInfo.subresourceRange.layerCount     = 1;

            imageViews[i] = device.GetHandle().createImageView(imageViewCreateInfo);
        }
    }

    vk::SurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats)
    {
        for (const auto& format : surfaceFormats)
        {
            const bool MATCHED_FORMAT = (format.format      == vk::Format::eR8G8B8A8Srgb);
            const bool MATCHED_COLOUR = (format.colorSpace  == vk::ColorSpaceKHR::eSrgbNonlinear);

            if (MATCHED_FORMAT && MATCHED_COLOUR)
                return format;
        }

        return surfaceFormats[0];
    }

    vk::PresentModeKHR SwapChain::choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes)
    {
        for (const auto& mode : presentModes)
        {
            if (mode == vk::PresentModeKHR::eMailbox)
                return mode;
        }

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D SwapChain::chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t windowWidth, uint32_t windowHeight)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
            return capabilities.currentExtent;


        vk::Extent2D actualExtent{ windowWidth, windowHeight };

        actualExtent.width  = std::clamp(actualExtent.width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

} // namespace RTR
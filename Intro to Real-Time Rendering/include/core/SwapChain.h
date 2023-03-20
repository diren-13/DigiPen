/*+======================================================================================
File:       SwapChain.h

Date:       16th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Defines an interface for SwapChains in Vulkan

Classes:    RTR::SwapChainProperties
            RTR::SwapChain
======================================================================================+*/

#pragma once

// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Device.h"
#include "Window.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates properties of a SwapChain.
    /// </summary>
    struct SwapChainProperties
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Extent2D            Extent;
        vk::SurfaceFormatKHR    Format;
        vk::PresentModeKHR      PresentMode;

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        SwapChainProperties();
    };

    /// <summary>
    /// Encapsulates a Swap Chain for a Vulkan Instance.
    /// </summary>
    class SwapChain
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        SwapChain();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::SwapchainKHR&            GetHandle()     const { return handle; }
        [[nodiscard]] inline const SwapChainProperties&         GetProperties() const { return properties; }

        [[nodiscard]] inline const std::vector<vk::Image>&      GetImages()     const { return images; }
        [[nodiscard]] inline const std::vector<vk::ImageView>&  GetImageViews() const { return imageViews; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Creates a surface for the swapchain.
        /// </summary>
        /// <param name="instance">
        /// The instance to create a surface from.
        /// </param>
        /// <param name="window">
        /// The window to create a surface from.
        /// </param>
        void CreateSurface  (Instance& instance, Window& window);
        /// <summary>
        /// Initialises a SwapChain.
        /// </summary>
        /// <param name="device">
        /// The device to initialise the SwapChain with.
        /// </param>
        /// <param name="window">
        /// The window to initialise the SwapChain with.
        /// </param>
        void Initialise     (Device& device, Window& window);
        /// <summary>
        /// Initalises a SwapChain from an old Swapchain.
        /// </summary>
        /// <param name="device">
        /// The device to initialise the swapchain with.
        /// </param>
        /// <param name="window">
        /// The window to initialise the SwapChain with.
        /// </param>
        void Initialise     (vk::SwapchainKHR oldSwapchain, Device& device, Window& window);
        /// <summary>
        /// Recreates the SwapChain.
        /// </summary>
        /// <param name="device">
        /// The device to recreate the swapchain with.
        /// </param>
        /// <param name="window">
        /// The window to recreate the SwapChain with.
        /// </param>
        void Recreate       (Device& device, Window& window);
        /// <summary>
        /// Destroys a SwapChain.
        /// </summary>
        /// <param name="instance">
        /// The instance to destroy a surface.
        /// </param>
        /// <param name="device">
        /// The device to destroy the swapchain.
        /// </param>
        void Destroy        (Instance& instance, Device& device) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::SwapchainKHR            handle;

        vk::SurfaceKHR              surface;
        bool                        hasSurface;
        SwapChainProperties         properties;

        std::vector<vk::Image>      images;
        std::vector<vk::ImageView>  imageViews;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void                        createSwapChain     (Device& device, const vk::SurfaceCapabilitiesKHR& capabilities);
        void                        createImageViews    (Device& device);

        static vk::SurfaceFormatKHR chooseSurfaceFormat (const std::vector<vk::SurfaceFormatKHR>& surfaceFormats);
        static vk::PresentModeKHR   choosePresentMode   (const std::vector<vk::PresentModeKHR>& presentModes);
        static vk::Extent2D         chooseExtent        (const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t windowWidth, uint32_t windowHeight);
    };

    
} // namespace RTR
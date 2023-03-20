/*+======================================================================================
File:       PhysicalDevice.h

Date:       8th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Defines an interface for interacting with Physical Devices using Vulkan.

Classes:    RTR::PhysicalDevice
            RTR::PhysicalDevices
=========================================================================================+*/

#pragma once

// Standard Libraries
#include <vector>
// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Instance.h"
#include "Queue.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Physical Device for a Vulkan instance.
    /// <br>
    /// It is responsible for handling GPU features, properties and queue families.
    /// </summary>
    class PhysicalDevice
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor for a Physical Device. Picks a suitable physical device from the
        /// attached instance.
        /// </summary>
        /// <param name="_instance">
        /// The instance for the physical device.
        /// </param>
        /// <param name="gpu">
        /// A Handle to the physical device encapsulated by the Vulkan API.
        /// </param>
        PhysicalDevice(Instance& _instance, vk::PhysicalDevice gpu);

        PhysicalDevice(const PhysicalDevice&)               = delete;
        PhysicalDevice(PhysicalDevice&&)                    = delete;
        PhysicalDevice& operator=(const PhysicalDevice&)    = delete;
        PhysicalDevice& operator=(PhysicalDevice&&)         = delete;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const Instance&                                GetInstance()               const { return *instance; }
        [[nodiscard]] inline const vk::PhysicalDevice&                      GetHandle()                 const { return handle; }
        [[nodiscard]] inline const vk::PhysicalDeviceFeatures&              GetFeatures()               const { return features; }
        [[nodiscard]] inline const vk::PhysicalDeviceProperties&            GetProperties()             const { return properties; }
        [[nodiscard]] inline const std::vector<vk::QueueFamilyProperties>&  GetQueueFamilyProperties()  const { return queueFamilyProperties; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        /// <summary>
        /// Gets the queue family for the physical device.
        /// </summary>
        /// <returns>
        /// The Queue indices for the physical device.
        /// </returns>
        Queue       FindQueueFamilies   ()  const;
        /// <summary>
        /// Checks if the Physical Device is suitable for use on this instance.
        /// </summary>
        /// <returns>
        /// True if the device is suitable.
        /// </returns>
        bool        IsDeviceSuitable    ()  const;
        /// <summary>
        /// Gets the memory type of the device
        /// </summary>
        /// <param name="typeFilter">
        /// A filter for the memory type
        /// </param>
        /// <param name="flags">
        /// Flags for the memory property type to find.
        /// </param>
        /// <returns>
        /// The memory type.
        /// </returns>
        uint32_t    FindMemoryType      (uint32_t typeFilter, vk::MemoryPropertyFlags flags) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Instance*                                           instance;
        vk::PhysicalDevice                                  handle;
        vk::PhysicalDeviceFeatures                          features;
        vk::PhysicalDeviceProperties                        properties;

        std::vector<vk::QueueFamilyProperties>              queueFamilyProperties;
    };
    
} // namespace RTR

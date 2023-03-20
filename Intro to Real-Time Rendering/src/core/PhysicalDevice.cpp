/*+======================================================================================
File:       PhysicalDevice.cpp

Date:       8th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the Physical Device class.

Functions:  RTR::PhysicalDevice::PhysicalDevice

            RTR::PhysicalDevice::FindQueueFamilies
            RTR::PhysicalDevice::IsDeviceSuitable
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core\PhysicalDevice.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/

    PhysicalDevice::PhysicalDevice(Instance& _instance, vk::PhysicalDevice gpu)
    : instance  { &_instance}
    , handle    { gpu }
    {
        features    = handle.getFeatures();
        properties  = handle.getProperties();

        std::stringstream ss;
        ss << "Found Device: " << properties.deviceName;
        Utility::LogMessage(ss.str().c_str());

        uint32_t queueFamilyCount = 0;
        handle.getQueueFamilyProperties(&queueFamilyCount, nullptr);

        queueFamilyProperties.resize(queueFamilyCount);
        handle.getQueueFamilyProperties(&queueFamilyCount, queueFamilyProperties.data());
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    Queue PhysicalDevice::FindQueueFamilies() const
    {
        Queue queue;

        int32_t i = 0;
        for (auto& queueFamily : queueFamilyProperties)
        {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                queue.FamilyIndex() = i;
                queue.Index() = i;
                break;
            }

            ++i;
        }

        return queue;
    }

    bool PhysicalDevice::IsDeviceSuitable() const
    {
        Queue index = FindQueueFamilies();
        return index.IsValid();
    }

    uint32_t PhysicalDevice::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags flags) const
    {
        vk::PhysicalDeviceMemoryProperties memoryProperties = handle.getMemoryProperties();

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
        {
            if (typeFilter & (1U << i) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
                return i;
        }

        throw std::runtime_error("[PhysicalDevice::FindMemoryType] Failed to find a suitable memory type!");
    }

    

} // namespace RTR
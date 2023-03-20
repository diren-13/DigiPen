/*+======================================================================================
File:       Device.cpp

Date:       14th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the Device class.

Functions:  RTR::Device::Device
            RTR::Device::~Device

            RTR::Device::WaitIdle
            RTR::Device::IsSurfaceSupported
            
            RTR::Device::GetSystemGPUs
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Standard Libraries
#include <set>
// Primary Header
#include "core\Device.h"
// Project Headers
#include "core\Queue.h"
#include "core\Descriptors.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Members                                                             */
    /*---------------------------------------------------------------------------------*/
    std::vector<std::unique_ptr<PhysicalDevice>> Device::gpus;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Device::Device(Instance& _instance)
    : instance          { &_instance }
    , gpu               { nullptr }
    , mainQueueIndex    { 0 }
    , transferQueueIndex{ 0 }
    {
        // ==============================================================================
        // Create Device
        // ==============================================================================

        // Add default device extensions
        deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        if (gpus.empty())
        {
            GetSystemGPUs();
        }

        // Get a suitable device
        for (const auto& d : gpus)
        {
            if (d->IsDeviceSuitable())
            {
                gpu = d.get();
                break;
            }
        }

        if (!gpu)
            throw std::runtime_error("[Device::Device] Failed to find a suitable Device!");

        // Check if device supports extensions
        uint32_t extensionCount = 0;
        gpu->GetHandle().enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
        gpu->GetHandle().enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : gpu->GetHandle().enumerateDeviceExtensionProperties()) 
        {
            requiredExtensions.erase(extension.extensionName);
        }

        if (!requiredExtensions.empty())
            throw std::runtime_error("[Device::Device] Device does not support default extensions!");


        const Queue QUEUE = gpu->FindQueueFamilies();

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        const std::set UNIQUE_QUEUE_FAMILIES =
        {
            QUEUE.GetFamilyIndex(),
            QUEUE.GetIndex()
        };


        float queuePriority = 1.0f;
        for (auto& queueFamily : UNIQUE_QUEUE_FAMILIES)
        {
            vk::DeviceQueueCreateInfo queueCreateInfo =
            {
                .queueFamilyIndex   = static_cast<uint32_t>(queueFamily),
                .queueCount         = 1,
                .pQueuePriorities   = &queuePriority
            };

            queueCreateInfos.emplace_back(queueCreateInfo);
        }

        const vk::DeviceCreateInfo DEVICE_CREATE_INFO =
        {
            .queueCreateInfoCount       = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos          = queueCreateInfos.data(),
            .enabledExtensionCount      = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames    = deviceExtensions.data(),
            .pEnabledFeatures           = &gpu->GetFeatures()
        };

        try
        {
            handle = gpu->GetHandle().createDevice(DEVICE_CREATE_INFO);
            Utility::LogMessage("Created a Logical Device!");
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Device::Device] Failed to Create a Logical Device!");
        }

        // ==============================================================================
        // Create Device Command Pool & Command Buffers
        // - used for transferring data
        // ==============================================================================

        mainQueueIndex = QUEUE.GetIndex();
        transferQueueIndex = QUEUE.GetFamilyIndex();

        graphicsQueue = handle.getQueue(static_cast<uint32_t>(QUEUE.GetFamilyIndex()), 0);
        presentQueue = handle.getQueue(static_cast<uint32_t>(QUEUE.GetIndex()), 0);

        vk::CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.flags             = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex  = transferQueueIndex;

        try
        {
            commandPool = handle.createCommandPool(commandPoolCreateInfo);
            Utility::LogMessage("Created a Command Pool!");

            const vk::CommandBufferAllocateInfo CMD_BUFFER_ALLOC_INFO =
            {
                .commandPool        = commandPool,
                .level              = vk::CommandBufferLevel::ePrimary,
                .commandBufferCount = 1
            };

            handle.allocateCommandBuffers(&CMD_BUFFER_ALLOC_INFO, &commandBuffer);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Device::Device] Failed to create a command pool!");
        }

        // ==============================================================================
        // Create Device Descriptor Pool
        // - used for allocating descriptor sets
        // ==============================================================================

        const uint32_t NUM_DESCRIPTORS_PER_POOL = 1000U;

        std::vector poolSizes =
        {
            vk::DescriptorPoolSize{static_cast<vk::DescriptorType>(Descriptor::Type::Uniform),      NUM_DESCRIPTORS_PER_POOL},
            vk::DescriptorPoolSize{static_cast<vk::DescriptorType>(Descriptor::Type::ImageSampler), NUM_DESCRIPTORS_PER_POOL}
        };

        const vk::DescriptorPoolCreateInfo POOL_CREATE_INFO =
        {
            .maxSets        = NUM_DESCRIPTORS_PER_POOL,
            .poolSizeCount  = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes     = poolSizes.data()
        };

        try
        {
            descriptorPool = handle.createDescriptorPool(POOL_CREATE_INFO);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Device::Device] Failed to create descriptor pool!");
        }
    }

    Device::Device(Device&& rhs)
    : instance          { rhs.instance }
    , graphicsQueue     { rhs.graphicsQueue }
    , presentQueue      { rhs.presentQueue }
    , mainQueueIndex    { rhs.mainQueueIndex }
    , transferQueueIndex{ rhs.transferQueueIndex }
    , commandPool       { rhs.commandPool }
    , commandBuffer     { rhs.commandBuffer }
    , descriptorPool    { rhs.descriptorPool }
    {
        std::swap(gpu, rhs.gpu);
        rhs.gpu = nullptr;

        std::swap(deviceExtensions, rhs.deviceExtensions);
    }

    Device& Device::operator=(Device&& rhs)
    {
        instance           = rhs.instance;
        graphicsQueue      = rhs.graphicsQueue;
        presentQueue       = rhs.presentQueue;
        mainQueueIndex     = rhs.mainQueueIndex;
        transferQueueIndex = rhs.transferQueueIndex;
        commandPool        = rhs.commandPool;
        commandBuffer      = rhs.commandBuffer;
        descriptorPool     = rhs.descriptorPool;

        std::swap(gpu, rhs.gpu);
        rhs.gpu = nullptr;

        std::swap(deviceExtensions, rhs.deviceExtensions);

        return *this;
    }

    Device::~Device()
    {
        handle.destroyDescriptorPool(descriptorPool);

        handle.freeCommandBuffers(commandPool, 1U, &commandBuffer);
        handle.destroyCommandPool(commandPool);

        handle.destroy();
        Utility::LogMessage("Destroyed a Device!");
    }


    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void Device::WaitIdle() const
    {
        handle.waitIdle();
    }

    bool Device::IsSurfaceSupported(vk::SurfaceKHR surface)
    {
        if (vk::Bool32 supported = gpu->GetHandle().getSurfaceSupportKHR(mainQueueIndex, surface); !supported)
            return false;

        return true;
    }

    std::vector<DescriptorSet> Device::AllocateDescriptors(std::vector<DescriptorSetLayout*>& layouts)
    {
        std::vector<DescriptorSet>              sets(layouts.size());
        std::vector<vk::DescriptorSetLayout>    vkLayouts(layouts.size());
        
        for (size_t i = 0; i < layouts.size(); ++i)
        {
            sets[i]         = std::move(DescriptorSet{ *this, *layouts[i] });
            vkLayouts[i]    = layouts[i]->GetHandle();
        }

        const vk::DescriptorSetAllocateInfo ALLOC_INFO =
        {
            .descriptorPool     = descriptorPool,
            .descriptorSetCount = static_cast<uint32_t>(vkLayouts.size()),
            .pSetLayouts        = vkLayouts.data()
        };

        std::vector<vk::DescriptorSet> vkSets;

        try
        {
            vkSets = handle.allocateDescriptorSets(ALLOC_INFO);
            // Assign vulkan handles to descriptor set objects
            for (size_t i = 0; i < vkSets.size(); ++i)
            {
                sets[i].Handle() = vkSets[i];
            }
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[DescriptorPool::Allocate] Failed to allocate descriptor sets!");
        }

        return sets;
    }

    void Device::BeginSingleTimeCommand()
    {
        const vk::CommandBufferBeginInfo BEGIN_INFO =
        {
            .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        };

        commandBuffer.begin(BEGIN_INFO);
    }

    void Device::EndSingleTimeCommand()
    {
        commandBuffer.end();

        const vk::SubmitInfo SUBMIT_INFO =
        {
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer
        };

        graphicsQueue.submit(1, &SUBMIT_INFO, nullptr);
        graphicsQueue.waitIdle();
    }

    void Device::CopyBuffer(vk::Buffer srcHandle, vk::Buffer dstHandle, vk::DeviceSize bufferSize)
    {
        const vk::BufferCopy COPY_REGION =
        {
            .srcOffset  = 0,
            .dstOffset  = 0,
            .size       = bufferSize
        };

        commandBuffer.copyBuffer(srcHandle, dstHandle, 1U, &COPY_REGION);
    }



    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/

    void Device::GetSystemGPUs()
    {
        // Initialise gpus of the system only once
        uint32_t deviceCount = 0;
        instance->GetHandle().enumeratePhysicalDevices(&deviceCount, nullptr);

        if (deviceCount < 1)
            throw std::runtime_error("[Device::GetSystemGPUs] Cannot find any GPUs for the instance!");

        std::vector<vk::PhysicalDevice> devices(deviceCount);
        instance->GetHandle().enumeratePhysicalDevices(&deviceCount, devices.data());

        for (auto& d : devices)
        {
            gpus.emplace_back(std::make_unique<PhysicalDevice>(*instance, d));
        }
    }


} // namespace RTR
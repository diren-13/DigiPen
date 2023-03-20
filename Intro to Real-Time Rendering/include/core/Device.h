/*+======================================================================================
File:       Device.h

Date:       14th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Defines an interface for interacting with Devices using Vulkan.

Classes:    RTR::Device
======================================================================================+*/

#pragma once

// Standard Libraries
#include <vector>
// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "PhysicalDevice.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class DescriptorSet;
    class DescriptorSetLayout;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Logical Device.
    /// </summary>
    class Device
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Device              (Instance& _instance);
        Device              (Device&& rhs);
        Device& operator=   (Device&& rhs);
        ~Device             ();

        Device(const Device&)               = delete;
        Device& operator=(const Device&)    = delete;
        

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::Device&          GetHandle()             const { return handle; }
        [[nodiscard]] inline const Instance&            GetInstance()           const { return *instance; }
        [[nodiscard]] inline const PhysicalDevice&      GetGPU()                const { return *gpu; }
        [[nodiscard]] inline const vk::PhysicalDevice   GetGPUHandle()          const { return gpu->GetHandle(); }
        [[nodiscard]] inline const vk::Queue&           GetGraphicsQueue()      const { return graphicsQueue; }
        [[nodiscard]] inline const vk::Queue&           GetPresentQueue()       const { return presentQueue; }
        [[nodiscard]] inline uint32_t                   GetMainQueueIndex()     const { return mainQueueIndex; }
        [[nodiscard]] inline uint32_t                   GetTransferQueueIndex() const { return transferQueueIndex; }
        [[nodiscard]] inline vk::CommandPool            GetCommandPool()        const { return commandPool; }
        [[nodiscard]] inline vk::CommandBuffer          GetCommandBuffer()      const { return commandBuffer; }
        [[nodiscard]] inline vk::DescriptorPool         GetDescriptorPool()     const { return descriptorPool; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void                        WaitIdle                () const;
        bool                        IsSurfaceSupported      (vk::SurfaceKHR surface);
        std::vector<DescriptorSet>  AllocateDescriptors     (std::vector<DescriptorSetLayout*>& layouts);

        void                        BeginSingleTimeCommand  ();
        void                        EndSingleTimeCommand    ();

        void                        CopyBuffer              (vk::Buffer srcHandle, vk::Buffer dstHandle, vk::DeviceSize bufferSize);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Device                  handle;

        Instance*                   instance;
        PhysicalDevice*             gpu;

        vk::Queue                   graphicsQueue;
        vk::Queue                   presentQueue;
        uint32_t                    mainQueueIndex;
        uint32_t                    transferQueueIndex;

        vk::CommandPool             commandPool;
        vk::CommandBuffer           commandBuffer;

        vk::DescriptorPool          descriptorPool;

        std::vector<const char*>    deviceExtensions;

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static std::vector<std::unique_ptr<PhysicalDevice>> gpus;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void GetSystemGPUs();
    };

} // namespace RTR
    
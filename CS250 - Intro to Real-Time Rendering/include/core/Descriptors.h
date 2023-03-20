/*+======================================================================================
File:       Descriptors.h

Date:       2nd April 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Descriptor, Descriptor Set and Descriptor Set Layout

Classes:    RTR::Descriptor
            RTR::DescriptorSet
            RTR::DescriptorSetLayout
======================================================================================+*/

#pragma once

// Standard Libraries
#include <vector>
// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Device.h"
#include "Buffer.h"
#include "Texture.h"
#include "TextureSampler.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    struct Descriptor
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Represents the type of the Descriptor.
        /// </summary>
        enum class Type
        {
            Uniform         = static_cast<int>(vk::DescriptorType::eUniformBuffer),
            ImageSampler    = static_cast<int>(vk::DescriptorType::eCombinedImageSampler)
        };
        /// <summary>
        /// Represents the shader stage where the descriptor will be sent to.
        /// </summary>
        enum class ShaderStage
        {
            Vertex          = static_cast<int>(vk::ShaderStageFlagBits::eVertex),
            Fragment        = static_cast<int>(vk::ShaderStageFlagBits::eFragment)
        };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Type        DescriptorType;
        ShaderStage Shader;

        uint32_t    BindingPoint;
        uint32_t    Count;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Descriptor(Type type, ShaderStage shader, uint32_t bindingPoint, uint32_t count = 1);
    };

    /// <summary>
    /// Encapsulates a layout for descriptor sets. This defines how descriptor sets must be
    /// structured.
    /// </summary>
    class DescriptorSetLayout
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        DescriptorSetLayout(Device& _device, const std::vector<Descriptor>& descriptorBindings);
        ~DescriptorSetLayout();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::DescriptorSetLayout&         GetHandle()         const   { return handle; }
        [[nodiscard]] inline const std::vector<Descriptor::Type>&   GetLayoutTypes()    const   { return layoutTypes; }

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Device*                                     device;

        vk::DescriptorSetLayout                     handle;

        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        std::vector<Descriptor::Type>               layoutTypes;
    };

    /// <summary>
    /// Encapsulates a descriptor set.
    /// </summary>
    class DescriptorSet
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        DescriptorSet           ();
        DescriptorSet           (Device& _device, DescriptorSetLayout& _layout);
        DescriptorSet           (DescriptorSet&& rhs);
        DescriptorSet& operator=(DescriptorSet&& rhs);


        DescriptorSet(const DescriptorSet&)             = delete;
        DescriptorSet& operator= (const DescriptorSet&) = delete;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline vk::DescriptorSet          GetHandle() const   { return handle; }
        [[nodiscard]] inline const DescriptorSetLayout& GetLayout() const   { return *layout; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline vk::DescriptorSet&         Handle()            { return handle; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void WriteBuffer        (const Buffer& buffer, uint32_t offset, uint32_t bindingPoint, uint32_t bufferSize);
        void WriteImage         (const Texture& texture, const TextureSampler& sampler, uint32_t bindingPoint);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        Device*                 device;
        DescriptorSetLayout*    layout;

        vk::DescriptorSet       handle;
    };
} // namespace RTR
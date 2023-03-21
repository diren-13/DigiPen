/*+======================================================================================
File:       Descriptor.cpp

Date:       8th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for Descriptors.

Functions:  RTR::Descriptor::Descriptor
            RTR::DescriptorSet::DescriptorSet
            RTR::DescriptorSetLayout::DescriptorSetLayout
            RTR::DescriptorSetLayout::~DescriptorSetLayout

            RTR::DescriptorSet::WriteBuffer
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core/Descriptors.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/
    Descriptor::Descriptor(Type type, ShaderStage shader, uint32_t bindingPoint, uint32_t count)
    : DescriptorType    { type }
    , Shader            { shader }
    , BindingPoint      { bindingPoint }
    , Count             { count }
    {}

    DescriptorSetLayout::DescriptorSetLayout(Device& _device, const std::vector<Descriptor>& descriptorBindings)
    : device    { &_device}
    {
        // Store all bindings
        for (const auto& descriptor : descriptorBindings)
        {
            vk::DescriptorSetLayoutBinding layoutBindings =
            {
                .binding            = descriptor.BindingPoint,
                .descriptorType     = static_cast<vk::DescriptorType>(descriptor.DescriptorType),
                .descriptorCount    = descriptor.Count,
                .stageFlags         = static_cast<vk::ShaderStageFlagBits>(descriptor.Shader)
            };

            bindings.emplace_back(layoutBindings);
            layoutTypes.emplace_back(descriptor.DescriptorType);
        }

        const vk::DescriptorSetLayoutCreateInfo LAYOUT_CREATE_INFO =
        {
            .bindingCount   = static_cast<uint32_t>(bindings.size()),
            .pBindings      = bindings.data()
        };

        try
        {
            handle = device->GetHandle().createDescriptorSetLayout(LAYOUT_CREATE_INFO);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[DescriptorSetLayout::DescriptorSetLayout] Failed to create a Descriptor Set Layout!");
        }
    }

    DescriptorSet::DescriptorSet()
    : device        { nullptr }
    , layout        { nullptr }
    {}

    DescriptorSet::DescriptorSet(Device& _device, DescriptorSetLayout& _layout)
    : device        { &_device }
    , layout        { &_layout }
    {}

    DescriptorSet::DescriptorSet(DescriptorSet&& rhs)
    : handle        { rhs.handle }
    {
        std::swap(device, rhs.device);
        rhs.device = nullptr;

        std::swap(layout, rhs.layout);
        rhs.layout = nullptr;
    }

    DescriptorSet& DescriptorSet::operator=(DescriptorSet&& rhs)
    {
        handle = rhs.handle;

        std::swap(device, rhs.device);
        rhs.device = nullptr;

        std::swap(layout, rhs.layout);
        rhs.layout = nullptr;

        return *this;
    }


    DescriptorSetLayout::~DescriptorSetLayout()
    {
        device->GetHandle().destroyDescriptorSetLayout(handle);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void DescriptorSet::WriteBuffer(const Buffer& buffer, uint32_t offset, uint32_t bindingPoint, uint32_t bufferSize)
    {
        const vk::DescriptorBufferInfo BUFFER_INFO =
        {
            .buffer     = buffer.GetHandle(),
            .offset     = offset,
            .range      = bufferSize
        };

        const vk::WriteDescriptorSet WRITE_DESCRIPTOR_SET =
        {
            .dstSet             = handle,
            .dstBinding         = bindingPoint,
            .dstArrayElement    = 0U,
            .descriptorCount    = 1U,
            .descriptorType     = static_cast<vk::DescriptorType>(Descriptor::Type::Uniform),
            .pBufferInfo        = &BUFFER_INFO
        };

        device->GetHandle().updateDescriptorSets(1U, &WRITE_DESCRIPTOR_SET, 0U, nullptr);
    }

    void DescriptorSet::WriteImage(const Texture& texture, const TextureSampler& sampler, uint32_t bindingPoint)
    {
        const vk::DescriptorImageInfo IMAGE_INFO =
        {
            .sampler        = sampler.GetHandle(),
            .imageView      = texture.GetImageView(),
            .imageLayout    = vk::ImageLayout::eShaderReadOnlyOptimal
        };

        const vk::WriteDescriptorSet WRITE_DESCRIPTOR_SET =
        {
            .dstSet             = handle,
            .dstBinding         = bindingPoint,
            .descriptorCount    = 1U,
            .descriptorType     = static_cast<vk::DescriptorType>(Descriptor::Type::ImageSampler),
            .pImageInfo         = &IMAGE_INFO
        };

        device->GetHandle().updateDescriptorSets(1U, &WRITE_DESCRIPTOR_SET, 0U, nullptr);
    }


} // namespace RTR
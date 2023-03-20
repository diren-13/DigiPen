/*+======================================================================================
File:       Texture.cpp

Date:       7th April 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for a Texture.

Functions:  
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core/Texture.h"
// Project Headers
#include "Utilities.h"
#include "core/Buffer.h"

#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>

using namespace tinyddsloader;

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/

    Texture::Texture(Device& _device)
    : device    { &_device }
    , loaded    { false }
    , type      { Type::Linear }
    , width     { 0 }
    , height    { 0 }
    {}

    Texture::Texture(Device& _device, const std::string& filepath, Type _type)
    : device    { &_device }
    , loaded    { false }
    , type      { _type }
    , width     { 0 }
    , height    { 0 }
    {
        if (loaded = Load(filepath, type); !loaded)
            throw std::runtime_error("[Texture::Texture] Failed to load texture!");
    }

    Texture::Texture(Device& _device, const std::filesystem::path& filepath, Type _type)
    : device    { &_device }
    , loaded    { false }
    , type      { _type }
    {
        if (loaded = Load(filepath, type); loaded)
            throw std::runtime_error("[Texture::Texture] Failed to load texture!");
    }

    Texture::Texture(Texture&& rhs)
    : loaded    { rhs.loaded }
    , type      { rhs.type }
    {
        std::swap(device, rhs.device);
        rhs.device = nullptr;
    }

    Texture& Texture::operator=(Texture&& rhs)
    {
        loaded = rhs.loaded;
        type = rhs.type;

        std::swap(device, rhs.device);
        rhs.device = nullptr;

        return *this;
    }


    Texture::~Texture()
    {
        device->GetHandle().destroyImageView(imageView);
        device->GetHandle().destroyImage(handle);
        device->GetHandle().freeMemory(imageMemory);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    bool Texture::Load(const std::string& filepath, Type _type)
    {
        if (loaded)
        {
            Utility::LogWarning("Texture has already has a loaded Texture!");
            return true;
        }

        DDSFile textureFile;
        const auto RESULT = textureFile.Load(filepath.c_str());
        if (RESULT != tinyddsloader::Success)
        {
            Utility::LogError("[Texture::Load] Failed to load a Texture!");
            return false;
        }

        width   = textureFile.GetWidth();
        height  = textureFile.GetHeight();

        // Raw data to map to GPU memory
        auto rawData = textureFile.GetImageData()->m_mem;
        const uint32_t NUM_BYTES = textureFile.GetImageData()->m_memSlicePitch;


        Buffer imageTransferBuffer{ *device, Buffer::Type::Staging, NUM_BYTES, 1 };

        const vk::MemoryMapFlags EMPTY_FLAG;
        void* mem = device->GetHandle().mapMemory(imageTransferBuffer.GetMemory(), 0, NUM_BYTES, EMPTY_FLAG);
        memcpy_s(mem, NUM_BYTES, rawData, NUM_BYTES);
        device->GetHandle().unmapMemory(imageTransferBuffer.GetMemory());

        // Convert format from DDS
        type = _type;
        // Select format
        vk::Format format;
        {
            auto ddsFormat = textureFile.GetFormat();

            switch (ddsFormat)
            {
                case DDSFile::DXGIFormat::R8G8B8A8_UNorm:
                case DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB:
                    format = (type == Type::Linear) ? vk::Format::eR8G8B8A8Unorm : vk::Format::eR8G8B8A8Srgb;
                    break;

                case DDSFile::DXGIFormat::B8G8R8A8_UNorm:
                case DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB:
                    format = (type == Type::Linear) ? vk::Format::eB8G8R8A8Unorm : vk::Format::eB8G8R8A8Srgb;
                    break;

                case DDSFile::DXGIFormat::BC1_UNorm:
                case DDSFile::DXGIFormat::BC1_UNorm_SRGB:
                    format = (type == Type::Linear) ? vk::Format::eBc1RgbaUnormBlock : vk::Format::eBc1RgbaSrgbBlock;
                    break;

                case DDSFile::DXGIFormat::BC2_UNorm:
                case DDSFile::DXGIFormat::BC2_UNorm_SRGB:
                    format = (type == Type::Linear) ? vk::Format::eBc2UnormBlock : vk::Format::eBc2SrgbBlock;
                    break;

                case DDSFile::DXGIFormat::BC3_UNorm:
                case DDSFile::DXGIFormat::BC3_UNorm_SRGB:
                    format = (type == Type::Linear) ? vk::Format::eBc3UnormBlock : vk::Format::eBc3SrgbBlock;
                    break;

                case DDSFile::DXGIFormat::BC5_UNorm:
                    format = (type == Type::Linear) ? vk::Format::eBc5UnormBlock : vk::Format::eUndefined;
                    break;

                default: format = vk::Format::eB8G8R8A8Srgb;
                    break;
            }
        }

        // Create texture image
        vk::ImageCreateInfo imageCreateInfo = 
        {
            .imageType      = vk::ImageType::e2D,
            .format         = format,
            .mipLevels      = 1U,
            .arrayLayers    = 1U,
            .samples        = vk::SampleCountFlagBits::e1,
            .tiling         = vk::ImageTiling::eOptimal,
            .usage          = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            .sharingMode    = vk::SharingMode::eExclusive,
            .initialLayout  = vk::ImageLayout::eUndefined
        };
        imageCreateInfo.extent.width    = width;
        imageCreateInfo.extent.height   = height;
        imageCreateInfo.extent.depth    = 1U;

        try
        {
            handle = device->GetHandle().createImage(imageCreateInfo);

            vk::MemoryRequirements memoryRequirements = device->GetHandle().getImageMemoryRequirements(handle);

            const vk::MemoryAllocateInfo ALLOC_INFO =
            {
                .allocationSize     = memoryRequirements.size,
                .memoryTypeIndex    = device->GetGPU().FindMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
            };

            imageMemory = device->GetHandle().allocateMemory(ALLOC_INFO);
            device->GetHandle().bindImageMemory(handle, imageMemory, 0);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Texture::Load] Failed to create image!");
        }

        transitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        copyBufferToImage(imageTransferBuffer.GetHandle());
        transitionLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

        createImageView(format);

        return true;
    }

    bool Texture::Load(const std::filesystem::path& filepath, Type _type)
    {
        return Load(filepath.string(), _type);
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/

    void Texture::transitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
    {
        device->BeginSingleTimeCommand();
        {
            vk::ImageMemoryBarrier imageMemoryBarrier = 
            {
                .oldLayout              = oldLayout,
                .newLayout              = newLayout,
                .srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED,
                .image                  = handle 
            };

            imageMemoryBarrier.subresourceRange.aspectMask      = vk::ImageAspectFlagBits::eColor;
            imageMemoryBarrier.subresourceRange.layerCount      = 1;
            imageMemoryBarrier.subresourceRange.levelCount      = 1;

            vk::PipelineStageFlags srcStage, dstStage;
            if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
            {
                imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

                srcStage    = vk::PipelineStageFlagBits::eTopOfPipe;
                dstStage    = vk::PipelineStageFlagBits::eTransfer;
            }
            else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
            {
                imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                srcStage    = vk::PipelineStageFlagBits::eTransfer;
                dstStage    = vk::PipelineStageFlagBits::eFragmentShader;
            }
            else
                throw std::invalid_argument("[Texture::transitionLayout] has an unsupported layout transition!");

            device->GetCommandBuffer().pipelineBarrier
            (
                srcStage, dstStage, 
                vk::DependencyFlags{}, 0U, nullptr, 0U, nullptr, 1U, &imageMemoryBarrier
            );
        }
        device->EndSingleTimeCommand();
    }

    void Texture::copyBufferToImage(vk::Buffer buffer)
    {
        device->BeginSingleTimeCommand();
        {
            vk::BufferImageCopy bufferImageCopy;
            bufferImageCopy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            bufferImageCopy.imageSubresource.layerCount = 1U;
            bufferImageCopy.imageExtent = vk::Extent3D{ width, height, 1 };

            device->GetCommandBuffer().copyBufferToImage(buffer, handle, vk::ImageLayout::eTransferDstOptimal, 1U, &bufferImageCopy);
        }
        device->EndSingleTimeCommand();
    }

    void Texture::createImageView(vk::Format format)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo = 
        {
            .image      = handle,
            .viewType   = vk::ImageViewType::e2D,
            .format     = format
        };

        imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageViewCreateInfo.subresourceRange.layerCount = 1U;
        imageViewCreateInfo.subresourceRange.levelCount = 1U;

        try
        {
            imageView = device->GetHandle().createImageView(imageViewCreateInfo);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Texture::createImageView] Failed to create an image view!");
        }
    }


} // namespace RTR
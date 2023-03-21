/*+======================================================================================
File:       TextureSampler.cpp

Date:       18th April 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a TextureSampler.

Functions:  
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core/TextureSampler.h"


namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/

    TextureSampler::TextureSampler()
    : device { nullptr }
    {}

    TextureSampler::TextureSampler(Device& _device)
    : device    { &_device }
    {
        const bool HAS_ANISOTROPY = device->GetGPU().GetFeatures().samplerAnisotropy;

        const vk::SamplerCreateInfo SAMPLER_INFO =
        {
            .magFilter                  = vk::Filter::eLinear,
            .minFilter                  = vk::Filter::eLinear,
            .mipmapMode                 = vk::SamplerMipmapMode::eLinear, 
            .addressModeU               = vk::SamplerAddressMode::eRepeat,
            .addressModeV               = vk::SamplerAddressMode::eRepeat,
            .addressModeW               = vk::SamplerAddressMode::eRepeat,
            .anisotropyEnable           = HAS_ANISOTROPY,
            .maxAnisotropy              = HAS_ANISOTROPY ? device->GetGPU().GetProperties().limits.maxSamplerAnisotropy : 1.0f,
            .compareEnable              = false,
            .compareOp                  = vk::CompareOp::eAlways,
            .borderColor                = vk::BorderColor::eFloatOpaqueBlack,
            .unnormalizedCoordinates    = false
        };

        try
        {
            handle = device->GetHandle().createSampler(SAMPLER_INFO);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[TextureSampler::TextureSampler] Failed to create a texture sampler!");
        }
    }

    TextureSampler::TextureSampler(TextureSampler&& rhs)
    : handle { rhs.handle }
    {
        std::swap(device, rhs.device);
        rhs.device = nullptr;
    }

    TextureSampler& TextureSampler::operator=(TextureSampler&& rhs)
    {
        handle = rhs.handle;

        std::swap(device, rhs.device);
        rhs.device = nullptr;

        return *this;
    }


    TextureSampler::~TextureSampler()
    {
        device->GetHandle().destroySampler(handle);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/
    
} // namespace RTR

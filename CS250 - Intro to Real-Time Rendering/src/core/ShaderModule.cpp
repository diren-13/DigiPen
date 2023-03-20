/*+======================================================================================
File:       ShaderModule.cpp

Date:       19th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the ShaderModule Classes.

Functions:  RTR::ShaderModule::ShaderModule

            RTR::ShaderModule::Initialise
            RTR::ShaderModule::Destroy
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Standard Libraries
#include <fstream>
#include <iterator>
// Primary Header
#include "core\ShaderModule.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/

    ShaderModule::ShaderModule(Device& _device, ShaderType type, const std::filesystem::path& filePath)
    : device    { &_device }
    , stage     { static_cast<vk::ShaderStageFlagBits>(type) }
    , filename  { filePath.filename().string() }
    {
        if (!device)
            Utility::LogError("[ShaderModule] No valid device attached to a ShaderModule!");

        // Read shader
        std::ifstream shaderFile{ filePath.string().c_str(), std::ios::ate | std::ios::binary};

        if (!shaderFile.is_open())
            throw std::runtime_error("[ShaderModule] Failed to open shader: " + filename);

        const size_t FILE_SIZE = shaderFile.tellg();
        source.resize(FILE_SIZE);
        shaderFile.seekg(0);
        std::copy_n(std::istreambuf_iterator(shaderFile), FILE_SIZE, source.begin());
    }


    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void ShaderModule::Initialise()
    {
        const vk::ShaderModuleCreateInfo SHADER_MODULE_CREATE_INFO =
        {
            .codeSize   = source.size(),
            .pCode      = reinterpret_cast<const uint32_t*>(source.data()),
        };

        if (!device)
            throw std::runtime_error("[ShaderModule] Cannot initialise a ShaderModule without an attached Device!");

        try
        {
            handle = device->GetHandle().createShaderModule(SHADER_MODULE_CREATE_INFO);
        }
        catch (vk::SystemError error)
        {
            throw std::runtime_error("Failed to create a ShaderModule!");
        }
    }

    void ShaderModule::Destroy()
    {
        if (!device)
            throw std::runtime_error("[ShaderModule] Cannot destroy a shader module without a valid Device!");

        device->GetHandle().destroyShaderModule(handle);
    }

} // namespace RTR
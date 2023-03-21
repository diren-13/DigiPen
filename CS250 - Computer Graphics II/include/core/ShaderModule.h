/*+======================================================================================
File:       ShaderModule.h

Date:       19th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a ShaderModule.

Classes:    RTR::ShaderModule
======================================================================================+*/

#pragma once

// Standard Libraries
#include <string>
#include <filesystem>
// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Device.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Encapsulates the type of the shader
    /// </summary>
    enum class ShaderType
    {
        Vertex      = static_cast<int>(vk::ShaderStageFlagBits::eVertex),
        Fragment    = static_cast<int>(vk::ShaderStageFlagBits::eFragment),
    };

    /// <summary>
    /// Encapsulates a shader to use in a Vulkan Pipeline.
    /// </summary>
    class ShaderModule
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor for ShaderModule.
        /// </summary>
        /// <param name="_device">
        /// The device the shader module will be created from.
        /// </param>
        /// <param name="type">
        /// The type of the shader.
        /// </param>
        /// <param name="filePath">
        /// The path to the shader. Expected format is native.
        /// </param>
        ShaderModule(Device& _device, ShaderType type, const std::filesystem::path& filePath);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::ShaderModule&        GetHandle()         const { return handle; }
        [[nodiscard]] inline const Device&                  GetDevice()         const { return *device; }
        [[nodiscard]] inline vk::ShaderStageFlagBits        GetShaderStage()    const { return stage; }
        [[nodiscard]] inline const std::string&             GetFilename()       const { return filename; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        inline void SetStage    (vk::ShaderStageFlagBits shaderStage)   { stage = shaderStage; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Initialises the Shader Module
        /// </summary>
        void Initialise ();
        /// <summary>
        /// Destroys the Shader Module.
        /// </summary>
        void Destroy    ();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::ShaderModule        handle;

        Device*                 device;
        vk::ShaderStageFlagBits stage;
        std::string             filename;
        std::vector<uint8_t>    source;
    };


} // namespace RTR
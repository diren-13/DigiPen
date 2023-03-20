/*+======================================================================================
File:       Instance.h

Date:       2nd February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for creating a Vulkan Instance.

Classes:    RTR::InstanceProperties
            RTR::Instance
======================================================================================+*/

#pragma once

// Standard Libraries
#include <vector>
#include <string>
#include <filesystem>
// External Dependencies
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class       PhysicalDevice;
    class       Device;
    class       ShaderModule;
    enum class  ShaderType;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates the properties of a Vulkan Instance.
    /// </summary>
    struct InstanceProperties
    {
    public:

        std::string                 AppName;
        std::string                 EngineName;

        std::vector<std::string>    Extensions;
        std::vector<std::string>    ValidationLayers;

        InstanceProperties
        (
            const std::string&          _appName            = "",
            const std::string&          _engineName         = "",
            std::vector<std::string>    _extensions         = {},
            std::vector<std::string>    _validationLayers   = {}
        );
    };

    /// <summary>
    /// Encapsulates an Instance for Vulkan
    /// </summary>
    class Instance
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor for an Instance.
        /// </summary>
        /// <param name="_debug">
        /// If debugging is enabled for the instance.
        /// </param>
        /// <param name="_renderDoc">
        /// If debugging with renderDoc is enabled for the instance.
        /// </param>
        /// <param name="_properties">
        /// Properties for the instance to be initialised with.
        /// </param>
        Instance(bool _debug = false, bool _renderDoc = false, const InstanceProperties& _properties = {});

        Instance(const Instance&)               = delete;
        Instance(Instance&&)                    = delete;
        Instance& operator=(const Instance&)    = delete;
        Instance& operator=(Instance&&)         = delete;

        ~Instance();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::Instance&    GetHandle() const { return instance; }
        [[nodiscard]] inline Device&                GetDevice() const { return *device; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        ShaderModule CreateShader(ShaderType shaderType, const std::filesystem::path& shaderFilepath);

        // TODO: Create Buffers
        // TODO: Create Texture
        // TODO: Create TextureSampler

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Instance                instance;
        vk::DebugUtilsMessengerEXT  debugMessenger;
        bool                        debug;

        Device*                     device;

        /*-----------------------------------------------------------------------------*/
        /* Static Members                                                              */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// A callback function for debug messages.
        /// </summary>
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback
        (
            VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT             messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void*                                       pUserData
        );

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        std::vector<const char*>    getExtensions                   ();
        std::vector<const char*>    getValidationLayers             ();
        void                        checkValidationLayersSupport    (const std::vector<const char*>& validationLayers);
    };

} // namespace RTR
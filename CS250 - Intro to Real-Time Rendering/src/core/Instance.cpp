/*+======================================================================================
File:       Instance.cpp

Date:       2nd February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the Instance Class.

Functions:  RTR::InstanceProperties::InstanceProperties
            RTR::Instance::Instance
            RTR::Instance::~Instance
            
            RTR::Instance::debugCallback
            RTR::Instance::getExtensions
            RTR::Instance::getValidationLayers
            RTR::Instance::checkValidationLayersSupport
======================================================================================+*/



// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core\Instance.h"
// Standard Libraries
#include <cstring>

/*-------------------------------------------------------------------------------------*/
/* Macros                                                                              */
/*-------------------------------------------------------------------------------------*/
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    InstanceProperties::InstanceProperties
    (
        const std::string&          _appName,
        const std::string&          _engineName,
        std::vector<std::string>    _extensions,
        std::vector<std::string>    _validationLayers
    )
    : AppName           { _appName }
    , EngineName        { _engineName }
    , Extensions        { std::move(_extensions) }
    , ValidationLayers  { std::move(_validationLayers) }
    {}


    Instance::Instance(bool _debug, bool _renderDoc, const InstanceProperties& _properties)
    : instance      { nullptr }
    , debugMessenger{ nullptr }
    , debug         { _debug }
    , device        { nullptr }
    {
        Utility::LogMessage("Initialising Vulkan Instance...");

        // Create the dynamic dispatcher for hpp functions
        vk::DynamicLoader dynamicLoader;
        const auto VK_GET_INSTANCE_PROC_ADDRESS = dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(VK_GET_INSTANCE_PROC_ADDRESS);
        vkGetInstanceProcAddr(nullptr, "");


        vk::ApplicationInfo appInfo =
        {
            .pApplicationName   = _properties.AppName.c_str(),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName        = _properties.AppName.c_str(),
            .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion         = VK_API_VERSION_1_0
        };

        // Get extensions
        std::vector<const char*> extensions;

        if (_properties.Extensions.empty())
        {
        #ifdef _DEBUG
            Utility::LogWarning("Using Default Extensions");
        #endif
            extensions = getExtensions();
        }
        else
        {
            for (const auto& ext : _properties.Extensions)
            {
                extensions.emplace_back(ext.c_str());
            }
        }

        // Get validation layers
        // Container is empty if no validation layers needed
        std::vector<const char*> validationLayers;
        if (debug)
        {
            if (_properties.ValidationLayers.empty())
            {
            #ifdef _DEBUG
                Utility::LogWarning("Using Default Validation Layers");
            #endif
                validationLayers = getValidationLayers();
            }
            else
            {
                for (const auto& layer : _properties.ValidationLayers)
                {
                    validationLayers.emplace_back(layer.c_str());
                }
            }

            if (_renderDoc)
            {
                if (auto rD = std::find(validationLayers.begin(), validationLayers.end(), "VK_LAYER_RENDERDOC_Capture"); rD == validationLayers.end())
                {
                    validationLayers.emplace_back("VK_LAYER_RENDERDOC_Capture");
                }
            }

            checkValidationLayersSupport(validationLayers);
        }

        vk::InstanceCreateInfo createInfo = 
        {
            .pApplicationInfo           = &appInfo,
            .enabledLayerCount          = static_cast<uint32_t>(validationLayers.size()),
            .ppEnabledLayerNames        = validationLayers.data(),
            .enabledExtensionCount      = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames    = extensions.data(),
        };

        // Create debug messenger
        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (debug)
        {
            using severityFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;
            using messageType  = vk::DebugUtilsMessageTypeFlagBitsEXT;

            debugCreateInfo.messageSeverity    = severityFlag::eVerbose | severityFlag::eWarning    | severityFlag::eError;
            debugCreateInfo.messageType        = messageType::eGeneral  | messageType::eValidation  | messageType::ePerformance;
            debugCreateInfo.pfnUserCallback    = debugCallback;
            debugCreateInfo.pUserData          = nullptr;

            createInfo.pNext = &debugCreateInfo;
        }

        try
        {
            instance = vk::createInstance(createInfo, nullptr);
            Utility::LogMessage("Created an Instance!");
            VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

            Utility::LogHeader("Vulkan Instance");

            if (debug)
            {
                debugMessenger = instance.createDebugUtilsMessengerEXT(debugCreateInfo);
                Utility::LogMessage("Created an Debug Messenger!");
            }

            device = new Device(*this);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Instance::Instance] Failed to create Vulkan instance!");
        }
    }

    Instance::~Instance()
    {
        if (device)
            delete device;

        if (debug)
        {
            instance.destroyDebugUtilsMessengerEXT(debugMessenger);
            Utility::LogMessage("Destroyed Debug Messenger!");
        }

        instance.destroy();
        Utility::LogMessage("Destroyed an Instance!");
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/
    ShaderModule Instance::CreateShader(ShaderType shaderType, const std::filesystem::path& shaderFilepath)
    {
        return ShaderModule(*device, shaderType, shaderFilepath);
    }


    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/

    vk::Bool32 Instance::debugCallback
    (
        VkDebugUtilsMessageSeverityFlagBitsEXT              messageSeverity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT    messageType,
        const VkDebugUtilsMessengerCallbackDataEXT*         pCallbackData,
        [[maybe_unused]] void*                              pUserData
    )
    {
        using severityFlag = vk::DebugUtilsMessageSeverityFlagBitsEXT;
        switch (static_cast<severityFlag>(messageSeverity))
        {
            case severityFlag::eWarning:
            {
                Utility::LogWarning(pCallbackData->pMessage);
                break;
            }
            case severityFlag::eError:
            {
                Utility::LogError(pCallbackData->pMessage);
                break;
            }
            default: break;
        }

        return VK_FALSE;
    }

    std::vector<const char*> Instance::getExtensions()
    {
        std::vector extensions =
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        };

        if (debug)
        {
            extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    std::vector<const char*> Instance::getValidationLayers()
    {
        std::vector validationLayers =
        {
             "VK_LAYER_KHRONOS_validation"
        };

        return validationLayers;
    }
    void Instance::checkValidationLayersSupport(const std::vector<const char*>& validationLayers)
    {
        vk::Result result;

        uint32_t layerCount = 0;
        result = vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

        if (result != vk::Result::eSuccess)
            throw std::runtime_error("[Instance::checkValidationLayersSupport] Failed to check support for Validation Layers!");

        std::vector<vk::LayerProperties> availableLayers(layerCount);
        result = vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        if (result != vk::Result::eSuccess)
            throw std::runtime_error("[Instance::checkValidationLayersSupport] Failed to check support for Validation Layers!");

        // Check if requested layers are available
        for (const auto& requestedLayer : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layer : availableLayers)
            {
                layerFound = static_cast<bool>(strcmp(requestedLayer, layer.layerName));

                if (layerFound)
                    break;
                    
            }

            if (!layerFound)
                throw std::runtime_error("[Instance::checkValidationLayersSupport] Requested Validation Layers not available!");
        }
    }
    
} // namespace RTR
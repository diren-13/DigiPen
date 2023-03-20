/*+======================================================================================
File:       pch.h

Date:       19th January 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Contains precompiled headers.
======================================================================================+*/

#pragma once

/*-------------------------------------------------------------------------------------*/
/* Standard Libraries                                                                  */
/*-------------------------------------------------------------------------------------*/
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>

// Undefine max and min to prevent conflict with std::numeric_limits
#undef max
#undef min

/*-------------------------------------------------------------------------------------*/
/* Vulkan Headers                                                                      */
/*-------------------------------------------------------------------------------------*/
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_NO_NODISCARD_WARNINGS

#ifndef VK_USE_PLATFORM_WIN32_KHR
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <codeanalysis\warnings.h>
#pragma warning (push)
#pragma warning (disable : ALL_CODE_ANALYSIS_WARNINGS)

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#pragma warning (pop)

/*-------------------------------------------------------------------------------------*/
/* GLM                                                                                 */
/*-------------------------------------------------------------------------------------*/

#include <glm.hpp>
#include <gtc/constants.hpp>
#include <gtx/transform.hpp>
#include <gtx/rotate_vector.hpp>

/*-------------------------------------------------------------------------------------*/
/* Project Headers                                                                     */
/*-------------------------------------------------------------------------------------*/
#include "Config.h"
#include "Utilities.h"

#include "core\Application.h"
#include "core\Window.h"
#include "core\Instance.h"
#include "core\Queue.h"
#include "core\PhysicalDevice.h"
#include "core\Device.h"
#include "core\SwapChain.h"
#include "core\RenderPass.h"
#include "core\ShaderModule.h"
#include "core\VertexDescriptor.h"
#include "core\Pipeline.h"
#include "core\Canvas.h"
#include "core\Buffer.h"
#include "core\Depthbuffer.h"
#include "core\Descriptors.h"
#include "core\Texture.h"
#include "core\TextureSampler.h"

#include "geometry/Vertex.h"
#include "geometry/Cube.h"



/*+======================================================================================
File:       RenderPass.h

Date:       20th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a RenderPass.

Classes:    RTR::RenderPass
======================================================================================+*/

#pragma once

// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "SwapChain.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Render Pass
    /// </summary>
    class RenderPass
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        RenderPass() = default;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::RenderPass&      GetHandle()     const { return handle; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Initialises a Render Pass.
        /// </summary>
        void Initialise     (Device& device, SwapChain& swapChain);
        /// <summary>
        /// Destroys a Render Pass.
        /// </summary>
        void Destroy        (Device& device) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Structures                                                             */
        /*-----------------------------------------------------------------------------*/
        vk::RenderPass  handle;
    };

} // namespace RTR
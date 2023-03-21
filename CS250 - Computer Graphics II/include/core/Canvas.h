/*+======================================================================================
File:       Canvas.h

Date:       8th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Rendering Canvas.

Classes:    RTR::Canvas
======================================================================================+*/

#pragma once

// Standard Library
#include <vector>
// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "SwapChain.h"
#include "RenderPass.h"
#include "Buffer.h"
#include "Pipeline.h"
#include "Depthbuffer.h"
#include "Descriptors.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Canvas for rendering.
    /// </summary>
    class Canvas
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Aliases                                                                */
        /*-----------------------------------------------------------------------------*/
        using FrameBuffers      = std::vector<vk::Framebuffer>;
        using CommandBuffers    = std::vector<vk::CommandBuffer>;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Canvas  (Instance& _instance, Window& _window);
        ~Canvas ();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const Instance&        GetInstance()       const   { return *instance; }
        [[nodiscard]] inline const Device&          GetDevice()         const   { return *device; }
        [[nodiscard]] inline const Window&          GetWindow()         const   { return *window; }
        [[nodiscard]] inline const SwapChain&       GetSwapChain()      const   { return swapChain; }
        [[nodiscard]] inline const RenderPass&      GetRenderPass()     const   { return renderPass; }
        [[nodiscard]] inline const vk::ClearValue&  GetClearValue()     const   { return clearColour; }
        [[nodiscard]] inline const FrameBuffers&    GetFrameBuffers()   const   { return frameBuffers; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline SwapChain&             SetSwapChain    ()                  { return swapChain; }
        [[nodiscard]] inline RenderPass&            SetRenderPass   ()                  { return renderPass; }
        [[nodiscard]] inline FrameBuffers&          SetFrameBuffers ()                  { return frameBuffers; }

        void                                        SetClearValue   (glm::vec4 value);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        bool StartRendering     ();
        void BindPipeline       (Pipeline& pipeline);
        void BindBuffer         (Buffer& buffer);
        void BindDescriptorSet  (DescriptorSet& descriptorSet);
        void SetPushConstant    (vk::ShaderStageFlags flags, void* data, uint32_t size, uint32_t offset);
        void Draw               (uint32_t vertexCount, uint32_t firstVertex = 0);
        void DrawIndexed        (uint32_t indexCount, uint32_t firstIndex = 0, uint32_t vertexOffset = 0);
        void EndRendering       ();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Instance*                       instance;
        Device*                         device;
        Window*                         window;

        size_t                          currentFrame;
        size_t                          maxConcurrentFrames;
        uint32_t                        imageIndex;

        Pipeline*                       renderPipeline;

        SwapChain                       swapChain;
        RenderPass                      renderPass;
        Depthbuffer                     depthbuffer;
        vk::ClearValue                  clearColour;

        vk::CommandPool                 pool;

        std::vector<vk::Framebuffer>    frameBuffers;
        std::vector<vk::CommandBuffer>  commandBuffers;

        std::vector<vk::Semaphore>      imagesAvailable;
        std::vector<vk::Semaphore>      rendersFinished;

        std::vector<vk::Fence>          isFrameRendering;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void createFramebuffers     ();
        void createCommandPool      ();
        void createCommandBuffers   ();
        void destroyCommandBuffers  ();
        void setupFrame             ();


    };
} // namespace RTR
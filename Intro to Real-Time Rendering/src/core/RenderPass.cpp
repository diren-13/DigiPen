/*+======================================================================================
File:       RenderPass.cpp

Date:       20th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for a RenderPass.

Functions:  RTR::RenderPass::RenderPass

            RTR::RenderPass::AttachSwapChain
            RTR::RenderPass::Initialise
            RTR::RenderPass::Destroy
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core/RenderPass.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void RenderPass::Initialise(Device& device, SwapChain& swapChain)
    {
        const vk::AttachmentDescription COLOUR_ATTACHMENT = 
        {
            .format         = swapChain.GetProperties().Format.format,
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR
        };

        const vk::AttachmentDescription DEPTH_ATTACHMENT =
        {
            .format         = Depthbuffer::FindDepthFormat(device),
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal
        };

        const vk::AttachmentReference COLOUR_ATTACHMENT_REF = 
        {
            .attachment = 0,
            .layout     = vk::ImageLayout::eColorAttachmentOptimal
        };

        const vk::AttachmentReference DEPTH_ATTACHMENT_REF =
        {
            .attachment = 1,
            .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal
        };

        const vk::SubpassDescription SUBPASS_DESCRIPTION =
        {
            .pipelineBindPoint          = vk::PipelineBindPoint::eGraphics,
            .colorAttachmentCount       = 1,
            .pColorAttachments          = &COLOUR_ATTACHMENT_REF,
            .pDepthStencilAttachment    = &DEPTH_ATTACHMENT_REF
        };

        const vk::SubpassDependency SUBPASS_DEPENDENCY =
        {
            .srcSubpass     = VK_SUBPASS_EXTERNAL,
            .dstSubpass     = 0,
            .srcStageMask   = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
            .dstStageMask   = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
            .dstAccessMask  = vk::AccessFlagBits::eColorAttachmentRead          | vk::AccessFlagBits::eColorAttachmentWrite            |
                              vk::AccessFlagBits::eDepthStencilAttachmentRead   | vk::AccessFlagBits::eDepthStencilAttachmentWrite
        };

        vk::AttachmentDescription attachments[2] = { COLOUR_ATTACHMENT, DEPTH_ATTACHMENT };

        const vk::RenderPassCreateInfo RENDER_PASS_INFO =
        {
            .attachmentCount    = 2U,
            .pAttachments       = attachments,
            .subpassCount       = 1U,
            .pSubpasses         = &SUBPASS_DESCRIPTION,
            .dependencyCount    = 1U,
            .pDependencies      = &SUBPASS_DEPENDENCY 
        };

        try
        {
            handle = device.GetHandle().createRenderPass(RENDER_PASS_INFO);
            Utility::LogMessage("Created a Render Pass");
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[RenderPass::Initialise] Failed to create a render pass!");
        }
    }

    void RenderPass::Destroy(Device& device) const
    {
        device.GetHandle().destroyRenderPass(handle);
    }


} // namespace RTR
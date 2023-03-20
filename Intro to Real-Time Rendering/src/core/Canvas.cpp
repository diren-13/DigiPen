/*+======================================================================================
File:       Canvas.cpp

Date:       8th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the Canvas Class.

Functions:  RTR::Canvas::Canvas
            RTR::Canvas::~Canvas

            RTR::Canvas::StartRendering
            RTR::Canvas::BindPipeline
            RTR::Canvas::BindBuffer
            RTR::Canvas::BindDescriptorSet
            RTR::Canvas::Draw
            RTR::Canvas::DrawIndexed
            RTR::Canvas::EndRendering

            RTR::Canvas::createFramebuffers
            RTR::Canvas::createCommandPool
            RTR::Canvas::createCommandBuffers
            RTR::Canvas::destroyCommandBuffers
            RTR::Canvas::setupFrame
======================================================================================+*/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "core/Canvas.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Canvas::Canvas(Instance& _instance, Window& _window)
    : instance              { &_instance }
    , device                { &_instance.GetDevice() }
    , window                { &_window }
    , currentFrame          { 0 }
    , maxConcurrentFrames   { 2 }
    , imageIndex            { 0 }
    , renderPipeline        { nullptr }
    , clearColour           { {{{0.0f, 0.0f, 0.0f, 1.0f}}} }
    {
        // Create Swap Chain
        swapChain.CreateSurface(*instance, *window);
        swapChain.Initialise(*device, *window);

        // Create Render Pass
        renderPass.Initialise(*device, swapChain);

        // Create Depth buffer
        depthbuffer.Initalise(*device, swapChain);

        // Create Framebuffers
        createFramebuffers();

        // Create Command Pool
        createCommandPool();
        createCommandBuffers();

        // Setup Frame
        imagesAvailable.resize(maxConcurrentFrames);
        rendersFinished.resize(maxConcurrentFrames);
        isFrameRendering.resize(maxConcurrentFrames);

        setupFrame();
    }

    Canvas::~Canvas()
    {
        // Destroy Depthbuffer
        depthbuffer.Destroy(*device);

        // Destroy Frame
        for (size_t i = 0; i < maxConcurrentFrames; ++i)
        {
            device->GetHandle().destroySemaphore(rendersFinished[i]);
            device->GetHandle().destroySemaphore(imagesAvailable[i]);
            device->GetHandle().destroyFence(isFrameRendering[i]);
        }

        // Destroy command pool
        destroyCommandBuffers();
        device->GetHandle().destroyCommandPool(pool);

        // Destroy Framebuffers
        for (auto fb : frameBuffers)
        {
            device->GetHandle().destroyFramebuffer(fb);
        }
        // Destroy Render Pass
        renderPass.Destroy(*device);
        // Destroy Swap Chain
        swapChain.Destroy(*instance, *device);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void Canvas::SetClearValue(glm::vec4 value)
    {
        clearColour = { {{{value.r, value.b, value.g, value.a }}} };
    }


    bool Canvas::StartRendering()
    {
        // Wait for current frame to finish rendering & be ready for use
        device->GetHandle().waitForFences(1, &isFrameRendering[currentFrame], true, std::numeric_limits<uint64_t>::max());

        try
        {
            auto result = device->GetHandle().acquireNextImageKHR(swapChain.GetHandle(), std::numeric_limits<uint64_t>::max(), imagesAvailable[currentFrame], nullptr);
            imageIndex = result.value;
        }
        catch (const vk::OutOfDateKHRError&)
        {
            device->WaitIdle();

            // Recreate
            const size_t BUFFER_COUNT = frameBuffers.size();
            for (size_t i = 0; i < BUFFER_COUNT; ++i)
            {
                device->GetHandle().destroyFramebuffer(frameBuffers[i]);
            }
            depthbuffer.Destroy(*device);
            renderPass.Destroy(*device);
            destroyCommandBuffers();

            swapChain.Recreate(*device, *window);
            renderPass.Initialise(*device, swapChain);
            depthbuffer.Initalise(*device, swapChain);
            createFramebuffers();
            createCommandBuffers();

            return false;
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Canvas::StartRendering] Failed to acquire swap chain image!");
        }

        // Reset fences 
        device->GetHandle().resetFences(1, &isFrameRendering[currentFrame]);

        // Reset command buffers
        commandBuffers[imageIndex].reset();

        const vk::CommandBufferBeginInfo BEGIN_INFO;

        try
        {
            commandBuffers[imageIndex].begin(&BEGIN_INFO);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Canvas::StartRendering] Failed to begin render pass recoding!");
        }

        // Set viewport
        vk::Viewport viewport =
        {
            .x          = 0.0f,
            .y          = 0.0f,
            .width      = static_cast<float>(window->GetWindowWidth()),
            .height     = static_cast<float>(window->GetWindowHeight()),
            .minDepth   = 0.0f,
            .maxDepth   = 1.0f  
        };

        // Set scissor
        vk::Rect2D scissor;
        scissor.offset.x    = 0;
        scissor.offset.y    = 0;
        scissor.extent      = swapChain.GetProperties().Extent;

        vk::RenderPassBeginInfo renderPassBeginInfo =
        {
            .renderPass = renderPass.GetHandle(),
            .framebuffer = frameBuffers[imageIndex]
        };

        vk::ClearValue clearValues[2]{};
        clearValues[0] = clearColour;
        clearValues[1].depthStencil.depth   = 1.0f;
        clearValues[1].depthStencil.stencil = 0U;

        renderPassBeginInfo.renderArea.offset.x = 0;
        renderPassBeginInfo.renderArea.offset.y = 0;
        renderPassBeginInfo.renderArea.extent   = swapChain.GetProperties().Extent;
        renderPassBeginInfo.clearValueCount     = 2;
        renderPassBeginInfo.pClearValues        = clearValues;

        commandBuffers[imageIndex].beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

        commandBuffers[imageIndex].setViewport(0, 1, &viewport);
        commandBuffers[imageIndex].setScissor(0, 1, &scissor);

        return true;
    }

    void Canvas::BindPipeline(Pipeline& pipeline)
    {
        renderPipeline = &pipeline;
        commandBuffers[imageIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetHandle());
    }

    void Canvas::BindBuffer(Buffer& buffer)
    {
        vk::Buffer     buffers[] = { buffer.GetHandle() };
        vk::DeviceSize offsets[] = { 0 };

        switch (buffer.GetType())
        {
            case Buffer::Type::Vertex:  commandBuffers[imageIndex].bindVertexBuffers(0, 1, buffers, offsets);                       break;
            case Buffer::Type::Index:   commandBuffers[imageIndex].bindIndexBuffer(buffer.GetHandle(), 0, vk::IndexType::eUint16);  break;

            default: break;
        }
    }

    void Canvas::BindDescriptorSet(DescriptorSet& descriptorSet)
    {
        vk::DescriptorSet handle = descriptorSet.GetHandle();

        commandBuffers[imageIndex].bindDescriptorSets
        (
            vk::PipelineBindPoint::eGraphics,
            renderPipeline->GetLayout(),
            0,
            1,
            &handle,
            0,
            nullptr
        );
    }

    void Canvas::SetPushConstant(vk::ShaderStageFlags flags, void* data, uint32_t size, uint32_t offset)
    {
        commandBuffers[imageIndex].pushConstants
        (
            renderPipeline->GetLayout(),
            flags,
            offset,
            size,
            data
        );
    }


    void Canvas::Draw(uint32_t vertexCount, uint32_t firstVertex)
    {
        if (!renderPipeline)
            throw std::runtime_error("[Canvas::Draw] Failed to bind render pipeline!");

        commandBuffers[imageIndex].draw(vertexCount, 1, firstVertex, 0);
    }

    void Canvas::DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t vertexOffset)
    {
        commandBuffers[imageIndex].drawIndexed(indexCount, 1, firstIndex, vertexOffset, 0);
    }

    void Canvas::EndRendering()
    {
        commandBuffers[imageIndex].endRenderPass();

        try
        {
            commandBuffers[imageIndex].end();
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Canvas::EndRendering] Failed to record command buffer!");
        }

        vk::PipelineStageFlags waitStages[1] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        const vk::SubmitInfo SUBMIT_INFO =
        {
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &imagesAvailable[currentFrame],
            .pWaitDstStageMask    = waitStages,

            .commandBufferCount   = 1,
            .pCommandBuffers      = &commandBuffers[imageIndex],

            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &rendersFinished[currentFrame]
        };

        try
        {
            device->GetGraphicsQueue().submit(1, &SUBMIT_INFO, isFrameRendering[currentFrame]);
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Canvas::EndRendering] Failed to submit draw command buffer!");
        }

        const vk::PresentInfoKHR PRESENT_INFO =
        {
            .waitSemaphoreCount         = 1,
            .pWaitSemaphores            = &rendersFinished[currentFrame],

            .swapchainCount             = 1,
            .pSwapchains                = &swapChain.GetHandle(),
            .pImageIndices              = &imageIndex,
            .pResults                   = nullptr 
        };

        auto result = device->GetPresentQueue().presentKHR(&PRESENT_INFO);

        device->GetPresentQueue().waitIdle();

        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || window->GetIsResized())
        {
            window->SetIsResized(false);
            device->WaitIdle();

            // Recreate
            const size_t BUFFER_COUNT = frameBuffers.size();
            for (size_t i = 0; i < BUFFER_COUNT; ++i)
            {
                device->GetHandle().destroyFramebuffer(frameBuffers[i]);
            }

            depthbuffer.Destroy(*device);
            renderPass.Destroy(*device);
            destroyCommandBuffers();

            swapChain.Recreate(*device, *window);

            renderPass.Initialise(*device, swapChain);
            depthbuffer.Initalise(*device, swapChain);
            createFramebuffers();
            createCommandBuffers();
        }
        else if (result != vk::Result::eSuccess)
        {
            throw std::runtime_error("[Canvas::EndRendering] Failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % maxConcurrentFrames;
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/

    void Canvas::createFramebuffers()
    {
        const size_t NUM_IMAGE_VIEWS = swapChain.GetImageViews().size();
        frameBuffers.resize(NUM_IMAGE_VIEWS);
        for (size_t i = 0; i < NUM_IMAGE_VIEWS; ++i)
        {
            vk::ImageView attachments [2] = { swapChain.GetImageViews()[i], depthbuffer.GetImageView() };

            const vk::FramebufferCreateInfo FRAMEBUFFER_CREATE_INFO =
            {
                .renderPass         = renderPass.GetHandle(),
                .attachmentCount    = 2,
                .pAttachments       = attachments,
                .width              = swapChain.GetProperties().Extent.width,
                .height             = swapChain.GetProperties().Extent.height,
                .layers             = 1
            };

            try
            {
                frameBuffers[i] = device->GetHandle().createFramebuffer(FRAMEBUFFER_CREATE_INFO);
                Utility::LogMessage("Created a framebuffer!");
            }
            catch (const vk::SystemError&)
            {
                throw std::runtime_error("[Canvas::Canvas] Failed to create a framebuffer!");
            }
        }
    }

    void Canvas::createCommandPool()
    {
        Queue indices = device->GetGPU().FindQueueFamilies();

        vk::CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.flags             = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex  = static_cast<uint32_t>(indices.GetFamilyIndex());

        try
        {
            pool = device->GetHandle().createCommandPool(commandPoolCreateInfo);
            Utility::LogMessage("Created a Command Pool!");
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[Canvas::createCommandPool] Failed to create a command pool!");
        }
    }

    void Canvas::createCommandBuffers()
    {
        const uint32_t NUM_BUFFERS = static_cast<uint32_t>(frameBuffers.size());
        commandBuffers.resize(NUM_BUFFERS);

        const vk::CommandBufferAllocateInfo CMD_BUFFER_ALLOC_INFO =
        {
            .commandPool        = pool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = NUM_BUFFERS
        };

        try
        {
            device->GetHandle().allocateCommandBuffers(&CMD_BUFFER_ALLOC_INFO, commandBuffers.data());
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[CommandPool::Initialise] Failed to allocate command buffers!");
        }
    }


    void Canvas::destroyCommandBuffers()
    {
        device->GetHandle().freeCommandBuffers(pool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }

    void Canvas::setupFrame()
    {
        const vk::SemaphoreCreateInfo SEMAPHORE_CREATE_INFO;

        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        for (size_t i = 0; i < maxConcurrentFrames; ++i)
        {
            try
            {
                imagesAvailable[i] = device->GetHandle().createSemaphore(SEMAPHORE_CREATE_INFO);
                Utility::LogMessage("Created a semaphore!");

                rendersFinished[i] = device->GetHandle().createSemaphore(SEMAPHORE_CREATE_INFO);
                Utility::LogMessage("Created a semaphore!");
            }
            catch (const vk::SystemError&)
            {
                throw std::runtime_error("[Canvas::setupFrame] Failed to create a semaphore!");
            }

            try
            {
                isFrameRendering[i] = device->GetHandle().createFence(fenceCreateInfo);
                Utility::LogMessage("Created a fence!");
            }
            catch (const vk::SystemError&)
            {
                throw std::runtime_error("[Canvas::setupFrame] Failed to create a fence!");
            }
        }
    }



} // namespace RTR
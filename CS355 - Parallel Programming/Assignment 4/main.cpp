/* Start Header *****************************************************************/ 
/*! \file   main.cpp
    \author Diren D Bharwani, diren.dbharwani, 390002520 
    \par    diren.dbharwani@digipen.edu
    \date   Nov 25, 2022 
    \brief  Copyright (C) 2022 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.
*/ 
/* End Header *******************************************************************/


#include "appBase.h"
#include "vkgltf.h"

#define ENABLE_VALIDATION true

class VulkanExample : public VkAppBase
{
public:
  bool splitScreen = true;

  struct {
    vks::Buffer tessControl, tessEval;
  } uniformBuffers;

  struct UBOTessControl {
    glm::vec3 center;
    float tessLevel = 64.0f;
  } uboTessControl;

  struct UBOTessEval {
    glm::mat4 projection;
    glm::mat4 modelView;
    glm::vec3 abc;
  } uboTessEval;

  struct Pipelines {
    VkPipeline solid;
    VkPipeline wireframe = VK_NULL_HANDLE;
  } pipelines;

  VkPipelineLayout pipelineLayout;
  VkDescriptorSet descriptorSet;
  VkDescriptorSetLayout descriptorSetLayout;

  VulkanExample() : VkAppBase(ENABLE_VALIDATION)
  {
    title = "Tessellation shader displacement";
    camera.type = Camera::CameraType::lookat;
    camera.setPosition(glm::vec3(0.0f, 0.0f, -5.25f));
    camera.setRotation(glm::vec3(-20.0f, 45.0f, 0.0f));
    camera.setPerspective(60.0f, (float)width * 0.5f / (float)height, 0.1f, 256.0f);
  }

  ~VulkanExample()
  {
    // Clean up used Vulkan resources
    // Note : Inherited destructor cleans up resources stored in base class
    vkDestroyPipeline(device, pipelines.solid, nullptr);
    if (pipelines.wireframe != VK_NULL_HANDLE) {
      vkDestroyPipeline(device, pipelines.wireframe, nullptr);
    };

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    uniformBuffers.tessControl.destroy();
    uniformBuffers.tessEval.destroy();
  }

  // Enable physical device features required for this example
  virtual void getEnabledFeatures()
  {
    // Tessellation shader support is required for this example
    if (deviceFeatures.tessellationShader) {
      enabledFeatures.tessellationShader = VK_TRUE;
    }
    else {
      vks::tools::exitFatal("Selected GPU does not support tessellation shaders!", VK_ERROR_FEATURE_NOT_PRESENT);
    }
    // Fill mode non solid is required for wireframe display
    if (deviceFeatures.fillModeNonSolid) {
      enabledFeatures.fillModeNonSolid = VK_TRUE;
    }
    else {
      splitScreen = false;
    }
  }

  void loadAssets()
  {
    //const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices | vkglTF::FileLoadingFlags::PreMultiplyVertexColors | vkglTF::FileLoadingFlags::FlipY;
    //plane.loadFromFile(getAssetPath() + "models/displacement_plane.gltf", vulkanDevice, queue, glTFLoadingFlags);
    //textures.colorHeightMap.loadFromFile(getAssetPath() + "textures/stonefloor03_color_height_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM, vulkanDevice, queue);
  }

  void buildCommandBuffers()
  {
    VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

    VkClearValue clearValues[2];
    clearValues[0].color = defaultClearColor;
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = vks::initializers::renderPassBeginInfo();
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
    {
      // Set target frame buffer
      renderPassBeginInfo.framebuffer = frameBuffers[i];

      VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));

      vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

      VkViewport viewport = vks::initializers::viewport(splitScreen ? (float)width * 0.5f : (float)width, (float)height, 0.0f, 1.0f);
      vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

      VkRect2D scissor = vks::initializers::rect2D(width, height, 0, 0);
      vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

      vkCmdSetLineWidth(drawCmdBuffers[i], 1.0f);

      vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

      vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.solid);
      vkCmdDraw(drawCmdBuffers[i], 1, 1, 0, 0);

      if (splitScreen)
      {
        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.wireframe);
        viewport.x = (float)width * 0.5f;
        vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);
        vkCmdDraw(drawCmdBuffers[i], 1, 1, 0, 0);
        //scissor.offset.x = width / 2;
        //vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);
      }

      //drawUI(drawCmdBuffers[i]);

      vkCmdEndRenderPass(drawCmdBuffers[i]);

      VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
    }
  }

  void setupDescriptorPool()
  {
    std::vector<VkDescriptorPoolSize> poolSizes = {
      vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
      vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
    };
    VkDescriptorPoolCreateInfo descriptorPoolInfo = vks::initializers::descriptorPoolCreateInfo(poolSizes, 2);
    VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool));
  }

  void setupDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
    {
      // Binding 0 : Tessellation control shader ubo
      vks::initializers::descriptorSetLayoutBinding(
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        0),
      // Binding 1 : Tessellation evaluation shader ubo
      vks::initializers::descriptorSetLayoutBinding(
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        1),
    };

    VkDescriptorSetLayoutCreateInfo descriptorLayout =
      vks::initializers::descriptorSetLayoutCreateInfo(
        setLayoutBindings.data(),
        static_cast<uint32_t>(setLayoutBindings.size()));

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &descriptorSetLayout));

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
      vks::initializers::pipelineLayoutCreateInfo(
        &descriptorSetLayout,
        1);

    VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout));
  }

  void setupDescriptorSet()
  {
    VkDescriptorSetAllocateInfo allocInfo = vks::initializers::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);
    VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));

    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
      // Binding 0 : Tessellation control shader ubo
      vks::initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uniformBuffers.tessControl.descriptor),
      // Binding 1 : Tessellation evaluation shader ubo
      vks::initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &uniformBuffers.tessEval.descriptor),
    };
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
  }

  void preparePipelines()
  {
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
      vks::initializers::pipelineInputAssemblyStateCreateInfo(
        VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
        0,
        VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
      vks::initializers::pipelineRasterizationStateCreateInfo(
        VK_POLYGON_MODE_FILL,
        VK_CULL_MODE_BACK_BIT,
        VK_FRONT_FACE_COUNTER_CLOCKWISE,
        0);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
      vks::initializers::pipelineColorBlendAttachmentState(
        0xf,
        VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
      vks::initializers::pipelineColorBlendStateCreateInfo(
        1,
        &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
      vks::initializers::pipelineDepthStencilStateCreateInfo(
        VK_TRUE,
        VK_TRUE,
        VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
      vks::initializers::pipelineViewportStateCreateInfo(1, 1, 0);

    VkPipelineMultisampleStateCreateInfo multisampleState =
      vks::initializers::pipelineMultisampleStateCreateInfo(
        VK_SAMPLE_COUNT_1_BIT,
        0);

    std::vector<VkDynamicState> dynamicStateEnables = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR,
      VK_DYNAMIC_STATE_LINE_WIDTH
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
      vks::initializers::pipelineDynamicStateCreateInfo(
        dynamicStateEnables.data(),
        static_cast<uint32_t>(dynamicStateEnables.size()),
        0);

    VkPipelineTessellationStateCreateInfo tessellationState =
      vks::initializers::pipelineTessellationStateCreateInfo(1);

    // Tessellation pipeline
    // Load shaders
    std::array<VkPipelineShaderStageCreateInfo, 4> shaderStages;
    shaderStages[0] = loadShader(getShadersPath() + "a4/ellipsoid.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = loadShader(getShadersPath() + "a4/ellipsoid.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    shaderStages[2] = loadShader(getShadersPath() + "a4/ellipsoid.tesc.spv", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    shaderStages[3] = loadShader(getShadersPath() + "a4/ellipsoid.tese.spv", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

    VkPipelineVertexInputStateCreateInfo vertexStateInfo = vks::initializers::pipelineVertexInputStateCreateInfo({}, {});

    VkGraphicsPipelineCreateInfo pipelineCI = vks::initializers::pipelineCreateInfo(pipelineLayout, renderPass);
    pipelineCI.pInputAssemblyState = &inputAssemblyState;
    pipelineCI.pRasterizationState = &rasterizationState;
    pipelineCI.pColorBlendState = &colorBlendState;
    pipelineCI.pMultisampleState = &multisampleState;
    pipelineCI.pViewportState = &viewportState;
    pipelineCI.pDepthStencilState = &depthStencilState;
    pipelineCI.pDynamicState = &dynamicState;
    pipelineCI.pTessellationState = &tessellationState;
    pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCI.pStages = shaderStages.data();
    pipelineCI.pVertexInputState = &vertexStateInfo;
      //vkglTF::Vertex::getPipelineVertexInputState({ vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::UV });

    // Solid pipeline
    VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.solid));
    if (deviceFeatures.fillModeNonSolid) {
      // Wireframe pipeline
      rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
      rasterizationState.cullMode = VK_CULL_MODE_NONE;
      VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.wireframe));
    }
  }

  // Prepare and initialize uniform buffer containing shader uniforms
  void prepareUniformBuffers()
  {
    // Tessellation evaluation shader uniform buffer
    VK_CHECK_RESULT(vulkanDevice->createBuffer(
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &uniformBuffers.tessEval,
      sizeof(uboTessEval)));

    // Tessellation control shader uniform buffer
    VK_CHECK_RESULT(vulkanDevice->createBuffer(
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      &uniformBuffers.tessControl,
      sizeof(uboTessControl)));

    // Map persistent
    VK_CHECK_RESULT(uniformBuffers.tessControl.map());
    VK_CHECK_RESULT(uniformBuffers.tessEval.map());
    
    updateUniformBuffers();
  }

  void updateUniformBuffers()
  {
    uboTessEval.projection = camera.matrices.perspective;
    uboTessEval.modelView = camera.matrices.view;
    uboTessEval.abc = glm::vec3(1.0f, 2.0f, 1.0f);
    memcpy(uniformBuffers.tessEval.mapped, &uboTessEval, sizeof(uboTessEval));

    // Tessellation control
    uboTessControl.center = glm::vec3(0.0f, 0.0f, 0.0f);
    memcpy(uniformBuffers.tessControl.mapped, &uboTessControl, sizeof(uboTessControl));
  }

  void draw()
  {
    VkAppBase::prepareFrame();

    // Command buffer to be submitted to the queue
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];

    // Submit to queue
    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    VkAppBase::submitFrame();
  }

  void prepare()
  {
    VkAppBase::prepare();
    loadAssets();
    prepareUniformBuffers();
    setupDescriptorSetLayout();
    preparePipelines();
    setupDescriptorPool();
    setupDescriptorSet();
    buildCommandBuffers();
    prepared = true;
  }

  virtual void render()
  {
    if (!prepared)
      return;
    draw();
    if (camera.updated) {
      updateUniformBuffers();
    }
  }
  virtual void OnUpdateUIOverlay(vks::UIOverlay* overlay)
  {
    //if (overlay->header("Settings")) {
    //  if (overlay->checkBox("Tessellation displacement", &displacement)) {
    //    updateUniformBuffers();
    //  }
    //  //if (overlay->inputFloat("Strength", &uboTessEval.tessStrength, 0.025f, 3)) {
    //  //  updateUniformBuffers();
    //  //}
    //  if (overlay->inputFloat("Level", &uboTessControl.tessLevel, 0.5f, 2)) {
    //    updateUniformBuffers();
    //  }
    //  if (deviceFeatures.fillModeNonSolid) {
    //    if (overlay->checkBox("Splitscreen", &splitScreen)) {
    //      buildCommandBuffers();
    //      updateUniformBuffers();
    //    }
    //  }

    //}
  }
};

VULKAN_EXAMPLE_MAIN()
/*+======================================================================================
File:       Pipeline.cpp

Date:       20th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the Pipeline Classes.

Functions:  RTR::VertexInputState::VertexInputState
            RTR::InputAssemblyState::InputAssemblyState
            RTR::ViewportState::ViewportState
            RTR::RasterisationState::RasterisationState
            RTR::MultisamplingState::MultisamplingState
            RTR::ColourBlendingState::ColourBlendingState
            RTR::PipelineState::PipelineState
            RTR::Pipeline::Pipeline
            RTR::Pipeline::~Pipeline
            RTR::GraphicsPipeline::GraphicsPipeline

            RTR::PipelineState::AddDynamicState
            RTR::PipelineState::RemoveDynamicState
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Standard Libraries
#include <algorithm>
// Primary Header
#include "core\Pipeline.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    InputAssemblyState::InputAssemblyState
    (
        Topology    _topology, 
        bool        _primitiveRestart
    )
    : topology          { _topology }
    , primitiveRestart  { _primitiveRestart }
    {}

    ViewportState::ViewportState
    (
        float   x, 
        float   y,
        float   width, 
        float   height,
        float   minDepth, 
        float   maxDepth, 
        int     offsetX, 
        int     offsetY
    )
    {
        viewport.x          = x;
        viewport.y          = y;
        viewport.width      = width;
        viewport.height     = height;
        viewport.minDepth   = minDepth;
        viewport.maxDepth   = maxDepth;

        scissor.offset.x    = offsetX;
        scissor.offset.y    = offsetY;
    }


    RasterisationState::RasterisationState
    (
        PolygonMode _polygonMode,      
        CullMode    _cullMode,         
        FrontFace   _frontFace,        
        bool        _depthClamp,       
        bool        _depthBias,        
        bool        _rasteriserDiscard,
        float       _lineWidth        
    )
    : polygonMode               { _polygonMode }
    , cullMode                  { _cullMode }
    , frontFace                 { _frontFace }
    , depthClamp                { _depthClamp }
    , depthBias                 { _depthBias }
    , rasteriserDiscard         { _rasteriserDiscard }
    , lineWidth                 { _lineWidth }
    , depthBiasConstantFactor   { 0.0f }
    , depthBiasClamp            { 0.0f }
    , depthBiasSlopeFactor      { 0.0f }
    {}

    MultisamplingState::MultisamplingState
    (
        bool        _sampleShading,   
        bool        _alphaToCoverage,
        bool        _alphaToOne,      
        SampleCount _sampleCount,     
        float       _minSampleShading
    )
    : sampleShading     { _sampleShading }
    , alphaToCoverage   { _alphaToCoverage }
    , alphaToOne        { _alphaToOne }
    , sampleCount       { _sampleCount }
    , minSampleShading  { _minSampleShading }
    {}

    ColourBlendingState::ColourBlendingState
    (
        bool _enableLogic,
        bool _enableBlending
    )
    : enableBlending    { _enableBlending }
    , enableLogic       { _enableLogic }
    , logicMode         { LogicMode::Copy }
    {
        if (enableLogic && enableBlending)
        {
            srcColourBlendFactor    = BlendFactor::SrcAlpha;
            dstColourBlendFactor    = BlendFactor::OneMinusSrcAlpha;
            srcAlphaBlendFactor     = BlendFactor::SrcAlpha;
            dstAlphaBlendFactor     = BlendFactor::OneMinusSrcAlpha;
        }
        else
        {
            srcColourBlendFactor    = BlendFactor::One;
            dstColourBlendFactor    = BlendFactor::Zero;
            srcAlphaBlendFactor     = BlendFactor::One;
            dstAlphaBlendFactor     = BlendFactor::Zero;
        }

        colourBlendMode = BlendMode::Add;
        alphaBlendMode  = BlendMode::Add;
    }

    DepthStencilState::DepthStencilState()
    : enableDepthTesting    { true }
    , enableDepthWriting    { true }
    , enableDepthBounds     { false }
    , enableStencilTesting  { false }
    , compareOp             { DepthOp::Less }
    , minDepth              { 0.0f }
    , maxDepth              { 1.0f }
    {}

    PipelineState::PipelineState()
    {
        dynamicStates.emplace(DynamicState::Viewport);
        dynamicStates.emplace(DynamicState::Scissor);
        dynamicStates.emplace(DynamicState::LineWidth);
    }

    Pipeline::Pipeline
    (
        Device&                 pipelineDevice,
        const PipelineState&    pipelineState
    )
    : device        { &pipelineDevice }
    , state         { pipelineState }
    , initialised   { false }
    {}

    Pipeline::~Pipeline()
    {
        if (!initialised)
            return;

        device->GetHandle().destroyPipeline(handle);
        Utility::LogMessage("Destroyed a pipeline!");

        device->GetHandle().destroyPipelineLayout(layout);
        Utility::LogMessage("Destroyed a pipeline layout!");
    }

    GraphicsPipeline::Setup::Setup()
    : renderPass            { nullptr }
    , pushConstant          { nullptr }
    , vertexDescriptor      { nullptr }
    {}

    GraphicsPipeline::GraphicsPipeline
    (
        Device&                 pipelineDevice,
        const Setup&            pipelineSetup,
        const PipelineState&    pipelineState
    )
    : Pipeline  { pipelineDevice, pipelineState }
    {
        if (!pipelineSetup.renderPass)
            throw std::runtime_error("[GraphicsPipeline::GraphicsPipeline] Cannot initialise a graphics pipeline without a render pass!");

        // Setup shaders
        if (pipelineSetup.shaders.size() == 0U)
            Utility::LogWarning("[GraphicsPipeline::GraphicsPipeline] Graphics Pipeline initialised without any shaders!");

        const size_t NUM_SHADERS = pipelineSetup.shaders.size();
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(NUM_SHADERS);

        for (size_t i = 0; i < NUM_SHADERS; ++i)
        {
            ShaderModule* shaderModule = pipelineSetup.shaders[i];

            shaderModule->Initialise();

            shaderStages[i].stage   = shaderModule->GetShaderStage();
            shaderStages[i].module  = shaderModule->GetHandle();
            shaderStages[i].pName   = "main";
        }


        if (pipelineSetup.vertexDescriptor)
        {
            // Copy bindings
            state.vis.bindings.resize(1);
            std::copy_n(pipelineSetup.vertexDescriptor->GetBindingDescriptions().cbegin(), 1, state.vis.bindings.begin());

            // Copy attributes
            const size_t NUM_ATTRIBUTES = pipelineSetup.vertexDescriptor->GetAttributes().size();
            state.vis.attributes.resize(NUM_ATTRIBUTES);
            std::copy_n(pipelineSetup.vertexDescriptor->GetAttributeDescriptions().cbegin(), NUM_ATTRIBUTES, state.vis.attributes.begin());
        }

        // Pipeline State
        const vk::PipelineVertexInputStateCreateInfo VIS_CREATE_INFO =
        {
            .vertexBindingDescriptionCount      = static_cast<uint32_t>(state.vis.bindings.size()),
            .pVertexBindingDescriptions         = state.vis.bindings.data(),
            .vertexAttributeDescriptionCount    = static_cast<uint32_t>(state.vis.attributes.size()),
            .pVertexAttributeDescriptions       = state.vis.attributes.data() 
        };

        // Setup Input Assembly
        vk::PipelineInputAssemblyStateCreateInfo IAS_CREATE_INFO =
        {
            .topology               = static_cast<vk::PrimitiveTopology>(state.ias.topology),
            .primitiveRestartEnable = state.ias.primitiveRestart 
        };

        // Setup Viewport
        vk::PipelineViewportStateCreateInfo VS_CREATE_INFO =
        {
            .viewportCount  = 1,
            .pViewports     = &state.vs.viewport,
            .scissorCount   = 1,
            .pScissors      = &state.vs.scissor
        };

        // Setup Rasteriser
        vk::PipelineRasterizationStateCreateInfo RS_CREATE_INFO =
        {
            .depthClampEnable           = state.rs.depthClamp,
            .rasterizerDiscardEnable    = state.rs.rasteriserDiscard,
            .polygonMode                = static_cast<vk::PolygonMode>(state.rs.polygonMode),
            .cullMode                   = static_cast<vk::CullModeFlagBits>(state.rs.cullMode),
            .frontFace                  = static_cast<vk::FrontFace>(state.rs.frontFace),
            .depthBiasEnable            = state.rs.depthBias,
            .depthBiasConstantFactor    = state.rs.depthBiasConstantFactor,
            .depthBiasClamp             = state.rs.depthBiasClamp,
            .depthBiasSlopeFactor       = state.rs.depthBiasSlopeFactor,
            .lineWidth                  = state.rs.lineWidth,
        };

        // Setup Multisampling
        vk::PipelineMultisampleStateCreateInfo MS_CREATE_INFO =
        {
            .rasterizationSamples   = static_cast<vk::SampleCountFlagBits>(state.ms.sampleCount),
            .sampleShadingEnable    = state.ms.sampleShading,
            .minSampleShading       = state.ms.minSampleShading,
            .pSampleMask            = nullptr,
            .alphaToCoverageEnable  = state.ms.alphaToCoverage,
            .alphaToOneEnable       = state.ms.alphaToOne 
        };

        // Setup Colour Blending
        vk::PipelineColorBlendAttachmentState CBAS_CREATE_INFO =
        {
            .blendEnable            = state.cbs.enableBlending,
            .srcColorBlendFactor    = static_cast<vk::BlendFactor>(state.cbs.srcColourBlendFactor),
            .dstColorBlendFactor    = static_cast<vk::BlendFactor>(state.cbs.dstColourBlendFactor),
            .colorBlendOp           = static_cast<vk::BlendOp>(state.cbs.colourBlendMode),
            .srcAlphaBlendFactor    = static_cast<vk::BlendFactor>(state.cbs.srcAlphaBlendFactor),
            .dstAlphaBlendFactor    = static_cast<vk::BlendFactor>(state.cbs.dstAlphaBlendFactor),
            .alphaBlendOp           = static_cast<vk::BlendOp>(state.cbs.alphaBlendMode),
            .colorWriteMask         = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                                    | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        };

        vk::PipelineColorBlendStateCreateInfo CBS_CREATE_INFO =
        {
            .logicOpEnable      = state.cbs.enableLogic,
            .logicOp            = static_cast<vk::LogicOp>(state.cbs.logicMode),
            .attachmentCount    = 1,
            .pAttachments       = &CBAS_CREATE_INFO,
        };

        vk::PipelineDepthStencilStateCreateInfo DSS_CREATE_INFO =
        {
            .depthTestEnable        = state.dss.enableDepthTesting,
            .depthWriteEnable       = state.dss.enableDepthWriting,
            .depthCompareOp         = static_cast<vk::CompareOp>(state.dss.compareOp),
            .depthBoundsTestEnable  = state.dss.enableDepthBounds,
            .stencilTestEnable      = state.dss.enableStencilTesting,
            .minDepthBounds         = state.dss.minDepth,
            .maxDepthBounds         = state.dss.maxDepth,
        };

        // Setup Dynamic States
        std::vector<vk::DynamicState> ds;
        for (auto dynamicState : state.dynamicStates)
        {
            ds.emplace_back(static_cast<vk::DynamicState>(dynamicState));
        }

        vk::PipelineDynamicStateCreateInfo DS_CREATE_INFO =
        {
            .dynamicStateCount  = static_cast<uint32_t>(state.dynamicStates.size()),
            .pDynamicStates     = ds.data()
        };
                    

        // Setup Layout
        const uint32_t NUM_LAYOUTS = static_cast<uint32_t>(pipelineSetup.descriptorSetLayouts.size());
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        for (auto& dsl : pipelineSetup.descriptorSetLayouts)
        {
            descriptorSetLayouts.emplace_back(dsl->GetHandle());
        }

        vk::PipelineLayoutCreateInfo layoutCreateInfo =
        {
            .setLayoutCount         = NUM_LAYOUTS,
            .pSetLayouts            = (NUM_LAYOUTS == 0) ? nullptr : descriptorSetLayouts.data(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges    = nullptr

        };
        

        if (pipelineSetup.pushConstant)
        {
            const vk::ShaderStageFlags VERT_AND_FRAG = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;

            // Setup push constants
            vk::PushConstantRange pushConstantRange =
            {
                .stageFlags = (pipelineSetup.pushConstant->allStages ? VERT_AND_FRAG : static_cast<vk::ShaderStageFlagBits>(pipelineSetup.pushConstant->shader)),
                .offset     = pipelineSetup.pushConstant->offset,
                .size       = pipelineSetup.pushConstant->size
            };

            layoutCreateInfo.pushConstantRangeCount = 1;
            layoutCreateInfo.pPushConstantRanges = &pushConstantRange;
        }

        try
        {
            layout = device->GetHandle().createPipelineLayout(layoutCreateInfo);
            Utility::LogMessage("Created a Pipeline layout!");
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[GraphicsPipeline::Initialise] Failed to create a pipeline layout!");
        }


        // Setup Pipeline
        const vk::GraphicsPipelineCreateInfo GRAPHICS_PIPELINE_CREATE_INFO =
        {
            .stageCount             = static_cast<uint32_t>(pipelineSetup.shaders.size()),
            .pStages                = shaderStages.data(),
            .pVertexInputState      = &VIS_CREATE_INFO,
            .pInputAssemblyState    = &IAS_CREATE_INFO,
            .pViewportState         = &VS_CREATE_INFO,
            .pRasterizationState    = &RS_CREATE_INFO,
            .pMultisampleState      = &MS_CREATE_INFO,
            .pDepthStencilState     = &DSS_CREATE_INFO,
            .pColorBlendState       = &CBS_CREATE_INFO,
            .pDynamicState          = &DS_CREATE_INFO,
            .layout                 = layout,
            .renderPass             = pipelineSetup.renderPass->GetHandle(),
            .subpass                = 0,
            .basePipelineHandle     = nullptr,
            .basePipelineIndex      = -1 
        };

        
        try
        {
            auto result = device->GetHandle().createGraphicsPipeline(nullptr, GRAPHICS_PIPELINE_CREATE_INFO);
            handle = result.value;
            Utility::LogMessage("Createad a Graphics Pipeline!");
            initialised = true;
        }
        catch (const vk::SystemError&)
        {
            throw std::runtime_error("[GraphicsPipeline::Initialise] Failed to create a pipeline!");
        }

        // Destroy shader modules
        for (auto& shader : pipelineSetup.shaders)
        {
            shader->Destroy();
        }
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    void PipelineState::AddDynamicState(DynamicState state)
    {
        dynamicStates.emplace(state);
    }

    void PipelineState::RemoveDynamicState(DynamicState state)
    {
        dynamicStates.erase(state);
    }
} // namespace RTR
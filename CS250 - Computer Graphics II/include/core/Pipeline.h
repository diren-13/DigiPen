/*+======================================================================================
File:       Pipeline.h

Date:       20th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Pipeline.

Classes:    RTR::VertexInputState
            RTR::InputAssemblyState
            RTR::ViewportState
            RTR::RasterisationState
            RTR::MultisamplingState
            RTR::ColourBlendingState
            RTR::PipelineState
            RTR::Pipeline
            RTR::GraphicsPipeline
======================================================================================+*/

#pragma once

// Standard Libraries
#include <vector>
#include <unordered_set>
// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers
#include "Device.h"
#include "RenderPass.h"
#include "VertexDescriptor.h"
#include "ShaderModule.h"
#include "Descriptors.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class Pipeline;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Encapsulates a Vertex Input State for a Pipeline
    /// </summary>
    struct VertexInputState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        std::vector<vk::VertexInputBindingDescription>      bindings;
        std::vector<vk::VertexInputAttributeDescription>    attributes;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        VertexInputState() = default;
    };

    struct InputAssemblyState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Encapsulates the type of the topology.
        /// </summary>
        enum class Topology
        {
            Point           = static_cast<int>(vk::PrimitiveTopology::ePointList),
            Line            = static_cast<int>(vk::PrimitiveTopology::eLineList),
            LineStrip       = static_cast<int>(vk::PrimitiveTopology::eLineStrip),
            Triangle        = static_cast<int>(vk::PrimitiveTopology::eTriangleList),
            TriangleStrip   = static_cast<int>(vk::PrimitiveTopology::eTriangleStrip),
            TriangleFan     = static_cast<int>(vk::PrimitiveTopology::eTriangleFan)
        };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Topology    topology;
        bool        primitiveRestart;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        InputAssemblyState
        (
            Topology    _topology           = Topology::Triangle, 
            bool        _primitiveRestart   = false
        );
    };

    struct ViewportState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Viewport viewport;
        vk::Rect2D   scissor;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        ViewportState
        (
            float   x           = 0.0f,
            float   y           = 0.0f,
            float   width       = 1600.0f,
            float   height      = 900.0f,
            float   minDepth    = 0.0f,
            float   maxDepth    = 0.0f,
            int     offsetX     = 0,
            int     offsetY     = 0
        );
    };

    struct RasterisationState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Encapsulates the polygon drawing mode.
        /// </summary>
        enum class PolygonMode
        {
            Fill                = static_cast<int>(vk::PolygonMode::eFill),
            Line                = static_cast<int>(vk::PolygonMode::eLine),
            Point               = static_cast<int>(vk::PolygonMode::ePoint)
        };
        /// <summary>
        /// Encapsulates the cull mode for rendering.
        /// </summary>
        enum class CullMode
        {
            None                = static_cast<int>(vk::CullModeFlagBits::eNone),
            Front               = static_cast<int>(vk::CullModeFlagBits::eFront),
            Back                = static_cast<int>(vk::CullModeFlagBits::eBack),
            FrontAndBack        = static_cast<int>(vk::CullModeFlagBits::eFrontAndBack),
        };
        /// <summary>
        /// Encapsulates the orientation of the points on a polygon.
        /// </summary>
        enum class FrontFace
        {
            CounterClockwise    = static_cast<int>(vk::FrontFace::eCounterClockwise),
            Clockwise           = static_cast<int>(vk::FrontFace::eClockwise)
        };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        PolygonMode polygonMode;
        CullMode    cullMode;
        FrontFace   frontFace;

        bool        depthClamp;
        bool        depthBias;
        bool        rasteriserDiscard;

        float       lineWidth;
        float       depthBiasConstantFactor;
        float       depthBiasClamp;
        float       depthBiasSlopeFactor;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        RasterisationState
        (
            PolygonMode _polygonMode        = PolygonMode::Fill,
            CullMode    _cullMode           = CullMode::Back,
            FrontFace   _frontFace          = FrontFace::CounterClockwise,
            bool        _depthClamp         = false,
            bool        _depthBias          = false,
            bool        _rasteriserDiscard  = false,
            float       _lineWidth          = 1.0f
        );
    };

    struct MultisamplingState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Encapsulates the sample count for the rasteriser.
        /// </summary>
        enum class SampleCount
        {
            One     = static_cast<int>(vk::SampleCountFlagBits::e1),
            Two     = static_cast<int>(vk::SampleCountFlagBits::e2),
            Four    = static_cast<int>(vk::SampleCountFlagBits::e4),
            Eight   = static_cast<int>(vk::SampleCountFlagBits::e8)
        };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        bool        sampleShading;
        bool        alphaToCoverage;
        bool        alphaToOne;

        SampleCount sampleCount;

        float       minSampleShading;
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        MultisamplingState
        (
            bool        _sampleShading      = false,
            bool        _alphaToCoverage    = false,
            bool        _alphaToOne         = false,
            SampleCount _sampleCount        = SampleCount::One,
            float       _minSampleShading   = 1.0f
        );
    };

    struct ColourBlendingState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Encapsulates the blending factor for colour blending.
        /// </summary>
        enum class BlendFactor
        {
            Zero                = static_cast<int>(vk::BlendFactor::eZero),
            One                 = static_cast<int>(vk::BlendFactor::eOne),
            SrcAlpha            = static_cast<int>(vk::BlendFactor::eSrcAlpha),
            OneMinusSrcAlpha    = static_cast<int>(vk::BlendFactor::eOneMinusSrcAlpha)
        };
        /// <summary>
        /// Encapsulates the blending mode for colour blending
        /// </summary>
        enum class BlendMode
        {
            Add                 = static_cast<int>(vk::BlendOp::eAdd)
        };
        /// <summary>
        /// Encapsulates the logic mode for colour blending
        /// </summary>
        enum class LogicMode
        {
            Copy                = static_cast<int>(vk::LogicOp::eCopy)
        };
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        bool        enableBlending;
        bool        enableLogic;

        LogicMode   logicMode;
        BlendFactor srcColourBlendFactor;
        BlendFactor dstColourBlendFactor;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor dstAlphaBlendFactor;
        BlendMode   colourBlendMode;
        BlendMode   alphaBlendMode;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        ColourBlendingState
        (
            bool _enableLogic       = false,
            bool _enableBlending    = false
        );
    };

    struct DepthStencilState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        enum class DepthOp
        {
            Less    = static_cast<int>(vk::CompareOp::eLess),
            More    = static_cast<int>(vk::CompareOp::eGreater)
        };

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        bool    enableDepthTesting;
        bool    enableDepthWriting;
        bool    enableDepthBounds;
        bool    enableStencilTesting;

        DepthOp compareOp;

        float   minDepth;
        float   maxDepth;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        DepthStencilState();
    };

    /// <summary>
    /// Encapsulates the state of the pipeline.
    /// </summary>
    class PipelineState
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        /// <summary>
        /// Encapsulates dynamic states for a pipeline.
        /// </summary>
        enum class DynamicState
        {
            Viewport    = static_cast<int>(vk::DynamicState::eViewport),
            Scissor     = static_cast<int>(vk::DynamicState::eScissor),
            LineWidth   = static_cast<int>(vk::DynamicState::eLineWidth)
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        PipelineState();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const VertexInputState  &  GetVertexInputState     () const { return vis; }
        [[nodiscard]] inline const InputAssemblyState&  GetInputAssemblyState   () const { return ias; }
        [[nodiscard]] inline const ViewportState     &  GetViewportState        () const { return vs; }
        [[nodiscard]] inline const RasterisationState&  GetRasterisationState   () const { return rs; }
        [[nodiscard]] inline const MultisamplingState&  GetMultisamplingState   () const { return ms; }
        [[nodiscard]] inline const ColourBlendingState& GetColourBlendingState  () const { return cbs; }
        [[nodiscard]] inline const DepthStencilState&   GetDepthStencilState    () const { return dss; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline void   SetVertexInputState        (const VertexInputState  & _vis)     { vis = _vis; }
        [[nodiscard]] inline void   SetInputAssemblyState      (const InputAssemblyState& _ias)     { ias = _ias; }
        [[nodiscard]] inline void   SetViewportState           (const ViewportState     & _vs)      { vs = _vs; }
        [[nodiscard]] inline void   SetRasterisationState      (const RasterisationState& _rs)      { rs = _rs; }
        [[nodiscard]] inline void   SetMultisamplingState      (const MultisamplingState& _ms)      { ms = _ms; }
        [[nodiscard]] inline void   SetColourBlendingState     (const ColourBlendingState& _cbs)    { cbs = _cbs; }
        [[nodiscard]] inline void   SetDepthStencilState       (const DepthStencilState& _dss)      { dss = _dss; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Adds a dynamic state to the pipeline. If the state already exists, nothing
        /// will change.
        /// </summary>
        /// <param name="state">
        /// The dynamic state to add to the pipeline.
        /// </param>
        void AddDynamicState(DynamicState state);
        /// <summary>
        /// Removes a dynamic state from the pipeline.
        /// </summary>
        /// <param name="state">
        /// The dynamic state to remove from the pipeline.
        /// </param>
        void RemoveDynamicState(DynamicState state);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        VertexInputState                    vis;
        InputAssemblyState                  ias;
        ViewportState                       vs;
        RasterisationState                  rs;
        MultisamplingState                  ms;
        ColourBlendingState                 cbs;
        DepthStencilState                   dss;

        std::unordered_set<DynamicState>    dynamicStates;

        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class Pipeline;
        friend class GraphicsPipeline;
    };

    /// <summary>
    /// Encapsulates a Base Class for a Vulkan Pipeline.
    /// </summary>
    class Pipeline
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Aliases                                                                */
        /*-----------------------------------------------------------------------------*/
        using Shaders           = std::vector<ShaderModule*>;
        using DescriptorLayouts = std::vector<DescriptorSetLayout*>;

        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Indicates which shader is in use.
        /// </summary>
        enum class Shader
        {
            Vertex      = static_cast<int>(vk::ShaderStageFlagBits::eVertex),
            Fragment    = static_cast<int>(vk::ShaderStageFlagBits::eFragment)
        };
        /// <summary>
        /// Represents a push constant 
        /// </summary>
        struct PushConstant
        {
            uint32_t    offset      = 0;
            uint32_t    size        = 0;
            Shader      shader      = Shader::Vertex;
            bool        allStages   = false;
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor of a Graphics Pipeline
        /// </summary>
        /// <param name="pipelineDevice">
        /// The device to initialise the pipeline with.
        /// </param>
        /// <param name="pipelineState">
        /// The state of the pipeline. If nothing is passed in, the current state is used.
        /// </param>
        Pipeline
        (
            Device&                 pipelineDevice, 
            const PipelineState&    pipelineState   = PipelineState{}
        );

        Pipeline(const Pipeline&)               = delete;
        Pipeline(Pipeline&&)                    = delete;
        Pipeline& operator=(const Pipeline&)    = delete;
        Pipeline& operator=(Pipeline&&)         = delete;

        ~Pipeline();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline const vk::Pipeline&        GetHandle()     const { return handle; }
        [[nodiscard]] inline const vk::PipelineLayout&  GetLayout()     const { return layout; }
        [[nodiscard]] inline const Device&              GetDevice()     const { return *device; }
        [[nodiscard]] inline const PipelineState&       GetState()      const { return state; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline PipelineState&             State()         { return state; }

    protected:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        vk::Pipeline        handle;
        vk::PipelineLayout  layout;

        Device*             device;
        PipelineState       state;
        bool                initialised;
    };

    /// <summary>
    /// Encapsulates a Graphics Pipeline for Vulkan.
    /// </summary>
    class GraphicsPipeline : public Pipeline
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Encapsulates parameters for setting up the graphics pipeline.
        /// </summary>
        struct Setup
        {
        public:
            /*-------------------------------------------------------------------------*/
            /* Data Members                                                            */
            /*-------------------------------------------------------------------------*/
            /// <param name="renderPass">
            /// The renderPass to initialise the pipeline with.
            /// </param>
            const RenderPass*       renderPass;
            /// <param name="pushConstant">
            /// The push constant to set up.
            /// </param>
            PushConstant*           pushConstant;
            /// <param name="vertexDescriptor">
            /// A vertex descriptor
            /// </param>
            VertexDescriptor*       vertexDescriptor;
            /// <param name="shaders">
            /// A container of shader modules the pipeline will use.
            /// </param>
            Shaders                 shaders;
            /// <summary>
            /// A container of descriptor set layouts the pipeline 
            /// </summary>
            DescriptorLayouts       descriptorSetLayouts;

            /*-------------------------------------------------------------------------*/
            /* Constructors & Destructor                                               */
            /*-------------------------------------------------------------------------*/
            Setup();
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor of a Graphics Pipeline
        /// </summary>
        /// <param name="pipelineDevice">
        /// The device to initialise the pipeline with.
        /// </param>
        /// <param name="pipelineSetup">
        /// The setup parameters for the graphics pipeline.
        /// </param>
        /// <param name="pipelineState">
        /// The state of the pipeline. If nothing is passed in, the current state is used.
        /// </param>
        GraphicsPipeline
        (
            Device&                 pipelineDevice,
            const Setup&            pipelineSetup,
            const PipelineState&    pipelineState       = PipelineState{}
        );

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
    };

} // namespace RTR

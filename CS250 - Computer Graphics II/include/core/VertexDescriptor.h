    /*+======================================================================================
File:       VertexDescriptor.h

Date:       7th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for a Vertex & VertexDescriptor.

Classes:    RTR::Vertex
            RTR::VertexDescriptor
======================================================================================+*/

#pragma once

// Standard Libraries
#include <vector>
// External Dependencies
#include <vulkan/vulkan.hpp>
#include <glm.hpp>
// Project Headers
#include "geometry/Vertex.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a descriptor for vertices.
    /// </summary>
    class VertexDescriptor
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr size_t AttributeDescriptionSize    = 4;
        static constexpr size_t BindingDescriptionSize      = 4;

        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// The format of an attribute.
        /// </summary>
        enum class Format
        {
            Float_2D,
            Float_3D,
            Float_4D
        };
        /// <summary>
        /// An attribute to be used in the VertexDescriptor
        /// </summary>
        struct Attribute
        {
            Format      format = Format::Float_3D;
            uint32_t    offset = 0;
        };

        /*-----------------------------------------------------------------------------*/
        /* Type Aliases                                                                */
        /*-----------------------------------------------------------------------------*/
        using AttributeDesc = std::array<vk::VertexInputAttributeDescription, AttributeDescriptionSize>;
        using BindingDesc   = std::array<vk::VertexInputBindingDescription, BindingDescriptionSize>;

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor for a VertexDescriptor.
        /// </summary>
        /// <param name="_attributes">
        /// A container of the attributes.
        /// </param>
        /// <param name="numAttributes">
        /// The number of attributes in a vertex.
        /// </param>
        /// <param name="_vertexSize">
        /// The size of the attribute in bytes.
        /// </param>
        VertexDescriptor(Attribute* _attributes, size_t numAttributes, size_t _vertexSize);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline uint32_t                       GetVertexSize()             const   { return vertexSize; }
        [[nodiscard]] inline const std::vector<Attribute>&  GetAttributes()             const   { return attributes; }
        [[nodiscard]] inline const AttributeDesc&           GetAttributeDescriptions()  const   { return attributeDescriptions; }
        [[nodiscard]] inline const BindingDesc&             GetBindingDescriptions()    const   { return bindingDescriptions; }

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        uint32_t                vertexSize;
        std::vector<Attribute>  attributes;
        AttributeDesc           attributeDescriptions;
        BindingDesc             bindingDescriptions;
    };

} // namespace RTR
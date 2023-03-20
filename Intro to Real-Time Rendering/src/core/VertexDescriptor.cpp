/*+======================================================================================
File:       VertexDescriptor.h

Date:       7th March 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for a VertexDescriptor.

Functions:  RTR::VertexDescriptor::VertexDescriptor
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Standard Libraries
#include <algorithm>
// Primary Header
#include "core\VertexDescriptor.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    VertexDescriptor::VertexDescriptor(Attribute* _attributes, size_t numAttributes, size_t _vertexSize)
    : vertexSize    {static_cast<uint32_t>(_vertexSize) }
    {
        // Set bindings (Interleaved)
        bindingDescriptions[0].binding      = 0;
        bindingDescriptions[0].stride       = vertexSize;
        bindingDescriptions[0].inputRate    = vk::VertexInputRate::eVertex;

        // Set attributes
        if (!_attributes)
            throw std::runtime_error("[VertexDescriptor::Initialise] Cannot initialise a vertex descriptor without any defined attributes!");

        attributes.resize(numAttributes);
        std::copy_n(_attributes, numAttributes, attributes.begin());

        // Set attributes
        for (size_t i = 0; i < attributes.size(); ++i)
        {
            attributeDescriptions[i].binding    = 0;
            attributeDescriptions[i].location   = static_cast<uint32_t>(i);
            attributeDescriptions[i].offset     = attributes[i].offset;
            
            switch (attributes[i].format)
            {
                case Format::Float_2D: attributeDescriptions[i].format = vk::Format::eR32G32Sfloat;         break;
                case Format::Float_3D: attributeDescriptions[i].format = vk::Format::eR32G32B32Sfloat;      break;
                case Format::Float_4D: attributeDescriptions[i].format = vk::Format::eR32G32B32A32Sfloat;   break;

                default: break;
            }
        }
    }
} // namespace RTR
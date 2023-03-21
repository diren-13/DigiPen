/*+======================================================================================
File:       Cube.h

Date:       20th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Contains Vertex Data for a Cube.
======================================================================================+*/

#pragma once

// Standard Libraries
#include <vector>
// Project Headers
#include "Vertex.h"
#include "core/VertexDescriptor.h"

namespace RTR::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Contains vertex data for a cube.
    /// </summary>
    struct Cube
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Vertex Data
        /// </summary>
        inline static const std::vector VERTICES =
        {
            // Front
            Vertex { .pos = { -0.5f, -0.5f,  0.5f }, .uv = { 0.0f, 1.0f }, .nml = {  0.0f,  0.0f,  1.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f, -0.5f,  0.5f }, .uv = { 1.0f, 1.0f }, .nml = {  0.0f,  0.0f,  1.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f,  0.5f,  0.5f }, .uv = { 1.0f, 0.0f }, .nml = {  0.0f,  0.0f,  1.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = { -0.5f,  0.5f,  0.5f }, .uv = { 0.0f, 0.0f }, .nml = {  0.0f,  0.0f,  1.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            // Back
            Vertex { .pos = { -0.5f, -0.5f, -0.5f }, .uv = { 0.0f, 1.0f }, .nml = {  0.0f,  0.0f, -1.0f }, .tan = { -1.0f, 0.0f,  0.0f } },
            Vertex { .pos = { -0.5f,  0.5f, -0.5f }, .uv = { 1.0f, 1.0f }, .nml = {  0.0f,  0.0f, -1.0f }, .tan = { -1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f,  0.5f, -0.5f }, .uv = { 1.0f, 0.0f }, .nml = {  0.0f,  0.0f, -1.0f }, .tan = { -1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f, -0.5f, -0.5f }, .uv = { 0.0f, 0.0f }, .nml = {  0.0f,  0.0f, -1.0f }, .tan = { -1.0f, 0.0f,  0.0f } },
            // Top
            Vertex { .pos = { -0.5f,  0.5f, -0.5f }, .uv = { 0.0f, 1.0f }, .nml = {  0.0f,  1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = { -0.5f,  0.5f,  0.5f }, .uv = { 1.0f, 1.0f }, .nml = {  0.0f,  1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f,  0.5f,  0.5f }, .uv = { 1.0f, 0.0f }, .nml = {  0.0f,  1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f,  0.5f, -0.5f }, .uv = { 0.0f, 0.0f }, .nml = {  0.0f,  1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            // Bottom
            Vertex { .pos = { -0.5f, -0.5f, -0.5f }, .uv = { 0.0f, 1.0f }, .nml = {  0.0f, -1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f, -0.5f, -0.5f }, .uv = { 1.0f, 1.0f }, .nml = {  0.0f, -1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = {  0.5f, -0.5f,  0.5f }, .uv = { 1.0f, 0.0f }, .nml = {  0.0f, -1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            Vertex { .pos = { -0.5f, -0.5f,  0.5f }, .uv = { 0.0f, 0.0f }, .nml = {  0.0f, -1.0f,  0.0f }, .tan = {  1.0f, 0.0f,  0.0f } },
            // Right
            Vertex { .pos = {  0.5f, -0.5f, -0.5f }, .uv = { 0.0f, 1.0f }, .nml = {  1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f, -1.0f } },
            Vertex { .pos = {  0.5f,  0.5f, -0.5f }, .uv = { 1.0f, 1.0f }, .nml = {  1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f, -1.0f } },
            Vertex { .pos = {  0.5f,  0.5f,  0.5f }, .uv = { 1.0f, 0.0f }, .nml = {  1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f, -1.0f } },
            Vertex { .pos = {  0.5f, -0.5f,  0.5f }, .uv = { 0.0f, 0.0f }, .nml = {  1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f, -1.0f } },
            // Left
            Vertex { .pos = { -0.5f, -0.5f, -0.5f }, .uv = { 0.0f, 1.0f }, .nml = { -1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f,  1.0f } },
            Vertex { .pos = { -0.5f, -0.5f,  0.5f }, .uv = { 1.0f, 1.0f }, .nml = { -1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f,  1.0f } },
            Vertex { .pos = { -0.5f,  0.5f,  0.5f }, .uv = { 1.0f, 0.0f }, .nml = { -1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f,  1.0f } },
            Vertex { .pos = { -0.5f,  0.5f, -0.5f }, .uv = { 0.0f, 0.0f }, .nml = { -1.0f,  0.0f,  0.0f }, .tan = {  0.0f, 0.0f,  1.0f } }
        };
        /// <summary>
        /// Index Data
        /// </summary>
        inline static const std::vector<uint16_t> INDICES =
        {
            0U,  1U,  2U,     0U,  2U,  3U,
            4U,  5U,  6U,     4U,  6U,  7U,
            8U,  9U,  10U,    8U, 10U, 11U,
            12U, 13U, 14U,    12U, 14U, 15U,
            16U, 17U, 18U,    16U, 18U, 19U,
            20U, 21U, 22U,    20U, 22U, 23U 
        };
    };


} // namespace RTR::Geometry
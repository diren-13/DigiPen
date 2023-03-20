/*+======================================================================================
File:       Vertex.h

Date:       20th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Contains Declaration for a Vertex
======================================================================================+*/

#pragma once

// External Dependencies
#include <glm.hpp>

namespace RTR::Geometry
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    struct Vertex
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        glm::vec3   pos;
        glm::vec2   uv;
        glm::vec3   nml;
        glm::vec3   tan;
    };


} // namespace RTR

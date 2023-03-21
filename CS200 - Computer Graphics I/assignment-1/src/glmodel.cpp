/************************************************************************************//*!
\file           glmodel.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 13, 2021
\brief          This file contains the implementation of class GLModel.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <glmodel.h>

#include <iostream>
#include <iomanip>      //
#include <sstream>
#include <array>

#include <dpml.h>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

GLModel::GLModel(GLenum mode)
: primitiveType (mode)
, drawCount     (0)
, rotation      (0.f)
{
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions                                                             */
/*-------------------------------------------------------------------------------------*/

void GLModel::InitWindowSizedQuad()
{
    // Constant to use
    const GLsizei   POS_SIZE = 2 * sizeof(GLfloat);                 // 2 floats: X,Y
    const GLsizei   TEX_SIZE = 2 * sizeof(GLfloat);                 // 2 floats: X,Y
    const GLsizei   STRIDE_SIZE = POS_SIZE + TEX_SIZE;

    // vertex data
    std::vector<GLfloat> vtx =
    {
        // pos (x, y)   // tex (x, y)                                                   
        -1.f,  -1.f,    0.f, 0.f,
         1.f,  -1.f,    1.f, 0.f,
        -1.f,   1.f,    0.f, 1.f,
         1.f,   1.f,    1.f, 1.f
    };

    // element indices
    std::vector<GLushort> idx = { 2, 0, 3, 1 };

    // Set model variables
    if (!primitiveType) { primitiveType = GL_TRIANGLE_STRIP; }
    drawCount = idx.size();
    
    // Send data to GPU
    glBindVertexArray(VAO);
    SetVBO(vtx);
    BindVBOPoints(STRIDE_SIZE);
    BindVBOTexCoords(POS_SIZE, STRIDE_SIZE);
    SetEBO(idx);
    glBindVertexArray(0);
}

bool GLModel::InitModel(const std::string& filename)
{
    bool meshLoadStatus = false;

    std::string filepath = "../meshes/" + filename + ".obj";

    #ifdef _DEBUG
    std::cout << "Loading: " <<  filepath << "..." << std::endl;
    #endif

    meshLoadStatus = DPML::parse_obj_mesh(filepath, posVtx, normalVtx, texVtx, idxVtx, true, true);

    if (meshLoadStatus == false)
    {
        std::cout << "Failed to load " << filename << "!" << std::endl;
        std::exit(EXIT_FAILURE);
        return meshLoadStatus;
    }
    else
    {
        #ifdef _DEBUG
        std::cout << "Loaded mesh " << filename << " --- SUCCESS!" << std::endl;
        #endif
    }

    // Resize device pos vector
    devicePos.resize(posVtx.size());

    // Map normalised normal from range of [-1,1] to [0,1]
    for (auto& normal : normalVtx)
    {
        normal.x = (normal.x + 1.f) * 0.5f;
        normal.y = (normal.y + 1.f) * 0.5f;
        normal.z = (normal.z + 1.f) * 0.5f;
    }

    // Generate random colours for triangles
    for (size_t i = 0; i < idxVtx.size() / 3; ++i)
    {
        Color randColor;
        for (int clr = 0; clr < 3; ++clr)
        {
            randColor.val[clr] = GLUtility::randInt(0, 255);
        }
        colour.emplace_back(randColor);
    }

    // Constants to use
    const GLsizei POS_SIZE = sizeof(glm::vec3) * posVtx.size();
    const GLsizei NML_SIZE = sizeof(glm::vec3) * normalVtx.size();
    const GLsizei TEX_SIZE = sizeof(glm::vec2) * texVtx.size();
    const GLsizei IDX_SIZE = sizeof(GLushort) * idxVtx.size();

    // Set model variables
    if (!primitiveType) { primitiveType = GL_TRIANGLES; }
    drawCount = idxVtx.size();

    // Print model loaded
    #ifndef _DEBUG
    std::cout   << "Model: "    << std::setw(7) << std::right << filename
                << " | Pos: "   << std::setw(7) << std::right << posVtx.size()
                << " | st: "    << std::setw(3) << std::right << 0
                << " | Nml: "   << std::setw(7) << std::right << normalVtx.size()
                << " | Tri: "   << std::setw(7) << std::right << idxVtx.size()
                << std::endl;
    #endif

    return meshLoadStatus;
}

void GLModel::Release()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

/*-------------------------------------------------------------------------------------*/
/* Private Member Functions                                                            */
/*-------------------------------------------------------------------------------------*/

void GLModel::BindVBOPoints(const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, POS_ATTRIB);
    glVertexArrayVertexBuffer(VAO, POS_BINDING_PT, VBO, 0, stride);
    glVertexArrayAttribFormat(VAO, POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, POS_ATTRIB, POS_BINDING_PT);
}

void GLModel::BindVBONormals(const GLsizei& offset, const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, NML_ATTRIB);
    glVertexArrayVertexBuffer(VAO, NML_BINDING_PT, VBO, offset, stride);
    glVertexArrayAttribFormat(VAO, NML_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, NML_ATTRIB, NML_BINDING_PT);
}

void GLModel::BindVBOColours(const GLsizei& offset, const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, CLR_ATTRIB);
    glVertexArrayVertexBuffer(VAO, CLR_BINDING_PT, VBO, offset, stride);
    glVertexArrayAttribFormat(VAO, CLR_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, CLR_ATTRIB, CLR_BINDING_PT);
}

void GLModel::BindVBOTexCoords(const GLsizei& offset, const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, TEX_ATTRIB);
    glVertexArrayVertexBuffer(VAO, TEX_BINDING_PT, VBO, offset, stride);
    glVertexArrayAttribFormat(VAO, TEX_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, TEX_ATTRIB, TEX_BINDING_PT);
}

/************************************************************************************//*!
\file           glmodel.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 13, 2021
\brief          This file contains the implementation of class Model.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <model.h>

#include <iostream>
#include <iomanip>      //
#include <sstream>
#include <array>

#include <dpml.h>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

Model::Model(GLenum mode)
: primitiveType (mode)
, drawCount     (0)
, primitiveCount(0)
, VAO           (0)
, position      (0.f)
, rotation      (0.f)
, scale         (1.f)
, modelToWorld  (0)
, VBO           (0)
, EBO           (0)
{}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions                                                             */
/*-------------------------------------------------------------------------------------*/

void Model::InitWindowSizedQuad()
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
    primitiveCount = drawCount - 2;

    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
    
    // Send data to GPU
    glBindVertexArray(VAO);
    SetVBO(vtx);
    BindVBOPoints(STRIDE_SIZE);
    BindVBOTexCoords(POS_SIZE, STRIDE_SIZE);
    SetEBO(idx);
    glBindVertexArray(0);
}

bool Model::InitModel(const std::string& fileName)
{
    bool meshLoadStatus = false;

    const std::string filepath = "../meshes/" + fileName;

    #ifdef _DEBUG
    std::cout << "Loading: " <<  filepath << "..." << std::endl;
    #endif

    meshLoadStatus = DPML::parse_obj_mesh(filepath, posVtx, normalVtx, texVtx, idxVtx, true, true);

    if (meshLoadStatus == false)
    {
        std::cout << "Failed to load " << filepath << "!" << std::endl;
        std::exit(EXIT_FAILURE);
        return meshLoadStatus;
    }
    else
    {
        #ifdef _DEBUG
        std::cout << "Loaded mesh " << filepath << " --- SUCCESS!" << std::endl;
        #endif
    }

    // Resize device pos vector
    devicePos.resize(posVtx.size());

    // Constants to use
    const GLsizei POS_SIZE = sizeof(glm::vec3) * posVtx.size();
    const GLsizei NML_SIZE = sizeof(glm::vec3) * normalVtx.size();
    const GLsizei TEX_SIZE = sizeof(glm::vec2) * texVtx.size();
    const GLsizei IDX_SIZE = sizeof(GLushort) * idxVtx.size();

    // Set model variables
    if (!primitiveType) { primitiveType = GL_TRIANGLES; }
    drawCount = idxVtx.size();
    primitiveCount = idxVtx.size() / Utility::TRI_PTS;

    // Print model loaded
    #ifndef _DEBUG

    std::string modelName = fileName.substr(0, fileName.find_first_of('.', 0));

    std::cout   << "Model: "                    << modelName                                        << std::endl
                << "Vertex Position Count: "    << std::setw(7) << std::right << posVtx.size()      << std::endl
                << "Vertex Texture  Count: "    << std::setw(7) << std::right << texVtx.size()      << std::endl
                << "Vertex Texture  Count: "    << std::setw(7) << std::right << normalVtx.size()   << std::endl
                << "Triangle Count:        "    << std::setw(7) << std::right << primitiveCount     << std::endl
                << std::endl;
    #endif

    return meshLoadStatus;
}

void Model::Release()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

/*-------------------------------------------------------------------------------------*/
/* Private Member Functions                                                            */
/*-------------------------------------------------------------------------------------*/

void Model::BindVBOPoints(const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, POS_ATTRIB);
    glVertexArrayVertexBuffer(VAO, POS_BINDING_PT, VBO, 0, stride);
    glVertexArrayAttribFormat(VAO, POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, POS_ATTRIB, POS_BINDING_PT);
}

void Model::BindVBONormals(const GLsizei& offset, const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, NML_ATTRIB);
    glVertexArrayVertexBuffer(VAO, NML_BINDING_PT, VBO, offset, stride);
    glVertexArrayAttribFormat(VAO, NML_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, NML_ATTRIB, NML_BINDING_PT);
}

void Model::BindVBOColours(const GLsizei& offset, const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, CLR_ATTRIB);
    glVertexArrayVertexBuffer(VAO, CLR_BINDING_PT, VBO, offset, stride);
    glVertexArrayAttribFormat(VAO, CLR_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, CLR_ATTRIB, CLR_BINDING_PT);
}

void Model::BindVBOTexCoords(const GLsizei& offset, const GLsizei& stride)
{
    glEnableVertexArrayAttrib(VAO, TEX_ATTRIB);
    glVertexArrayVertexBuffer(VAO, TEX_BINDING_PT, VBO, offset, stride);
    glVertexArrayAttribFormat(VAO, TEX_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, TEX_ATTRIB, TEX_BINDING_PT);
}

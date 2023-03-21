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

#include <iostream>
#include <sstream>
#include <array>

#include <glmodel.h>

#include <glapp.h>
#include <glutility.h>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

GLModel::GLModel(GLenum mode)
: primitiveType (mode)
, drawCount     (0)
, primitiveCount(0)
{
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions                                                             */
/*-------------------------------------------------------------------------------------*/

void GLModel::InitQuad()
{
    // Constant to use
    const GLsizei   POS_SIZE = 2 * sizeof(GLfloat);                 // 2 floats: X,Y
    const GLsizei   CLR_SIZE = 3 * sizeof(GLfloat);                 // 3 floats: R,G,B
    const GLsizei   TEX_SIZE = 2 * sizeof(GLfloat);                 // 2 floats: X,Y
    const GLsizei   STRIDE_SIZE = POS_SIZE + CLR_SIZE + TEX_SIZE;

    // Randomised colours for each vertex
    std::vector<glm::vec3> randClr;
    for (int i = 0; i < 4; ++i)
    {
        glm::vec3 colour(GLUtility::randFloat(), GLUtility::randFloat(), GLUtility::randFloat());
        randClr.emplace_back(colour);
    }

    // vertex data
    std::vector<GLfloat> vtx =
    {
        // pos (x, y)   // colour (r, g, b)                           // tex (x, y)                                                   
        -1.f,  -1.f,    randClr[0].r, randClr[0].g, randClr[0].b,     0.f, 0.f,
         1.f,  -1.f,    randClr[1].r, randClr[1].g, randClr[1].b,     1.f, 0.f,
        -1.f,   1.f,    randClr[2].r, randClr[2].g, randClr[2].b,     0.f, 1.f,
         1.f,   1.f,    randClr[3].r, randClr[3].g, randClr[3].b,     1.f, 1.f
    };

    // element indices
    std::vector<GLushort> idx = { 2, 0, 3, 1 };

    // Set model variables
    if (!primitiveType) { primitiveType = GL_TRIANGLE_STRIP; }
    drawCount = idx.size();
    primitiveCount = drawCount - 2;
    
    // Bind VBO to VAO
    glBindVertexArray(VAO);
    SetVBO(vtx);
    BindVBOPoints(STRIDE_SIZE);
    BindVBOColours(POS_SIZE, STRIDE_SIZE);
    BindVBOTexCoords(POS_SIZE + CLR_SIZE, STRIDE_SIZE);
    SetEBO(idx);
    glBindVertexArray(0);
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
    glVertexArrayAttribFormat(VAO, POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, POS_ATTRIB, POS_BINDING_PT);
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

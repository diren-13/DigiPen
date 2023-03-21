/************************************************************************************//*!
\file           glmodel.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 4, 2021
\brief          This file contains the declaration of class GLModel.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>   

#include <glslshader.h>     // GLSLShader class

struct Controller;          // forward declaration of GLController

/********************************************************************************//*!
@brief    Encapsulates a model made up of geometric primitives.
*//*********************************************************************************/
class GLModel
{
  public:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    GLenum	primitiveType;	    // The primitive type to be rendered
    GLuint  drawCount;          // Num of vertices of a primitive type to be rendered
    GLuint  primitiveCount;     // Num of primitives in an instance of the struct 
    GLuint  VAO;                // Handle to the Model's VAO
    GLuint  VBO;                // Handle to the Model's VBO
    GLuint  EBO;                // Handle to the Model's EBO

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    static const GLuint POS_ATTRIB = 0;         // Attribute index for position vertex data
    static const GLuint CLR_ATTRIB = 1;         // Attribute index for colour vertex data
    static const GLuint TEX_ATTRIB = 2;         // Attribute index for texture coordinate vertex data

    static const GLuint POS_BINDING_PT = 0;     // Binding index for position attributes
    static const GLuint CLR_BINDING_PT = 1;     // Binding index for colour attributes
    static const GLuint TEX_BINDING_PT = 2;     // Binding index for texture coordinate attributes

  public:
    /*-----------------------------------------------------------------------------*/
    /* Constructors                                                                */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Default constructor for GLModel.

    @param[in]    mode
        The primitive type of the model.
    *//*****************************************************************************/
    GLModel(GLenum mode = 0);

    /*-----------------------------------------------------------------------------*/
    /* Member Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief     Sets the model to be a quad made up of triangles using
               GL_TRIANGLE_STRIPS
    *//*****************************************************************************/
    void InitQuad();

    /****************************************************************************//*!
    @brief     Returns buffers back to GPU.
    *//*****************************************************************************/
    void Release();

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief    Function to bind attributes from the VBO to the VAO. Use this when
              storing vertex attributes as AoS (Array of Structures) or position is
              the only attribute.

              Default data type is GLfloat.

    @param[in]    vtxData
        A vector containing the vertex data.
    *//*****************************************************************************/
    template <typename t = GLfloat>
    void SetVBO(std::vector<t> vtxData);

    /****************************************************************************//*!
    @brief    Function to bind an EBO with a VAO.

              Default data type is GLushort.

    @param[in]    indexData
        The vector containing the indices for drawing elements.
    *//*****************************************************************************/
    template <typename t = GLushort>
    void SetEBO(std::vector<t> indexData);

    /****************************************************************************//*!
    @brief    Function to bind 2D points stored in the VBO to the VAO.
              Stored in attribute index 0. Binded using binding point 0.

              This should be called before 

    @param[in]    stride
        The number of bytes to the next attribute data.
    *//*****************************************************************************/
    void BindVBOPoints(const GLsizei& stride);

    /****************************************************************************//*!
    @brief    Function to bind colour values stored in the VBO to the VAO.
              Stored in attribute index 1. Binded using binding point 1.

              This function expects BindVBOPoints to be called first.

    @param[in]    offset
        The size of any previous vertex data preceding this.
    @param[in]    stride
        The number of bytes to the next attribute data.
    *//*****************************************************************************/
    void BindVBOColours(const GLsizei& offset, const GLsizei& stride);

    /****************************************************************************//*!
    @brief    Function to bind texture coordinate values stored in the VBO to the VAO.
              Stored in attribute index 2. Binded using binding point 2.

              This function expects BindVBOPoints to be called first.

    @param[in]    offset
        The size of any previous vertex data preceding this.
    @param[in]    stride
        The number of bytes to the next attribute data.
    *//*****************************************************************************/
    void BindVBOTexCoords(const GLsizei& offset, const GLsizei& stride);
};


/*-------------------------------------------------------------------------------------*/
/* Templated Member Functions                                                          */
/*-------------------------------------------------------------------------------------*/

template <typename t>
void GLModel::SetVBO(std::vector<t> vertexData)
{
    const GLsizei VTX_SIZE = sizeof(t) * vertexData.size();

    glNamedBufferStorage(VBO, VTX_SIZE, vertexData.data(), GL_DYNAMIC_STORAGE_BIT);
}

template <typename t>
void GLModel::SetEBO(std::vector<t> indexData)
{
    const GLsizeiptr IDX_SIZE = sizeof(t) * indexData.size();

    glNamedBufferStorage(EBO, IDX_SIZE, indexData.data(), GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(VAO, EBO);
}
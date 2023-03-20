/************************************************************************************//*!
\file           glmodel.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 4, 2021
\brief          This file contains the declaration of class Model.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>	    // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>   

#include <glslshader.h>     
#include <utility.h>        // Colour

/********************************************************************************//*!
@brief    Encapsulates a model made up of geometric primitives.
*//*********************************************************************************/
class Model
{
  public:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    GLenum	primitiveType;	    // The primitive type to be rendered
    GLuint  drawCount;          // Num of vertices of a primitive type to be rendered
    GLuint  primitiveCount;     // Number of triangles being drawn
    GLuint  VAO;                // Handle to the Model's VAO

    // Variables specific to this assignment
    std::vector<glm::vec3>  posVtx;         // normalised position vertex data
    std::vector<glm::vec3>  normalVtx;      // per vertex normal data
    std::vector<glm::vec2>  texVtx;         // texture mapping coordinates data
    std::vector<GLushort>   idxVtx;         // drawing indices
    std::vector<glm::vec3>  devicePos;      // window coordinates
    std::vector<Colour>     colour;         // number of colours = number of triangles 

    glm::vec3               position;       // Position of the model
    float                   rotation;       // Rotation of the model
    glm::vec3               scale;          // Scale of the model
    glm::mat4               modelToWorld;   // Transformation matrix for the model to the world

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    GLuint      VBO;            // Handle to the Model's VBO
    GLuint      EBO;            // Handle to the Model's EBO

    static const GLuint POS_ATTRIB = 0;         // Attribute index for position vertex data
    static const GLuint NML_ATTRIB = 1;         // Attribute index for per vertex normal
    static const GLuint CLR_ATTRIB = 2;
    static const GLuint TEX_ATTRIB = 3;         // Attribute index for texture coordinate vertex data

    static const GLuint POS_BINDING_PT = 0;     // Binding index for position attributes
    static const GLuint NML_BINDING_PT = 1;     // Binding index for per vertex normal attributes
    static const GLuint CLR_BINDING_PT = 2;     // Binding index for colour attributes
    static const GLuint TEX_BINDING_PT = 3;     // Binding index for texture coordinate attributes

  public:
    /*-----------------------------------------------------------------------------*/
    /* Constructors                                                                */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Default constructor for Model.

    @param[in]    mode
        The primitive type of the model.
    *//*****************************************************************************/
    Model(GLenum mode = 0);

    /*-----------------------------------------------------------------------------*/
    /* Member Functions                                                            */
    /*-----------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief    Sets the model to be a window-sized quad made up of triangles using
              GL_TRIANGLE_STRIPS. Data is laid out as AOS.
    *//*****************************************************************************/
    void InitWindowSizedQuad();

    /****************************************************************************//*!
    @brief    Initialises a model.

    @param[in]  fileName
        The file name of the model.
    *//*****************************************************************************/
    bool InitModel(const std::string& fileName);

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
    template <typename T>
    void SetVBO(const std::vector<T>& vtxData);

    /****************************************************************************//*!
    @brief    Function to bind an EBO with a VAO.

              Default data type is GLushort.

    @param[in]    indexData
        The vector containing the indices for drawing elements.
    *//*****************************************************************************/
    template <typename T = GLushort>
    void SetEBO(std::vector<T> indexData);

    /****************************************************************************//*!
    @brief    Function to bind 2D points stored in the VBO to the VAO.
              Stored in attribute index 0. Binded using binding point 0.

              This should be called before any other attribute.

    @param[in]    stride
        The number of bytes to the next attribute data.
    *//*****************************************************************************/
    void BindVBOPoints(const GLsizei& stride);

    /****************************************************************************//*!
    @brief    Function to bind per vertex normal values stored in the VBO to the VAO.
              Stored in attribute index 1. Binded using binding point 1.

              This function expects BindVBOPoints to be called first.

    @param[in]    offset
        The size of any previous vertex data preceding this.
    @param[in]    stride
        The number of bytes to the next attribute data.
    *//*****************************************************************************/
    void BindVBONormals(const GLsizei& offset, const GLsizei& stride);

    /****************************************************************************//*!
    @brief    Function to bind colour vertex values stored in the VBO to the VAO.
              Stored in attribute index 2. Binded using binding point 2.

              This function expects BindVBOPoints to be called first.

    @param[in]    offset
        The size of any previous vertex data preceding this.
    @param[in]    stride
        The number of bytes to the next attribute data.
    *//*****************************************************************************/
    void BindVBOColours(const GLsizei& offset, const GLsizei& stride);

    /****************************************************************************//*!
    @brief    Function to bind texture coordinate values stored in the VBO to the VAO.
              Stored in attribute index 3. Binded using binding point 3.

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

template <typename T>
void Model::SetVBO(const std::vector<T>& vtxData)
{
    const size_t VTX_SIZE = sizeof(T) * vtxData.size();

    glNamedBufferStorage(VBO, VTX_SIZE, vtxData.data(), GL_DYNAMIC_STORAGE_BIT);  
}

template <typename T>
void Model::SetEBO(std::vector<T> indexData)
{
    const GLsizeiptr IDX_SIZE = sizeof(T) * indexData.size();

    glNamedBufferStorage(EBO, IDX_SIZE, indexData.data(), GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(VAO, EBO);
}
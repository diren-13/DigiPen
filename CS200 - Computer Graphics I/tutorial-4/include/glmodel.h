/************************************************************************************//*!
\file           glmodel.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 4, 2021
\brief          This file contains the declaration of class Model to be used with GLApp.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <fstream>

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>      

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

    static const GLuint POS_BINDING_PT = 0;     // Binding index for position attributes
    static const GLuint CLR_BINDING_PT = 1;     // Binding index for colour attributes

  public:
    /*-----------------------------------------------------------------------------*/
    /* Constructors                                                                */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Default constructor for GLModel.
    *//*****************************************************************************/
    GLModel();

    /****************************************************************************//*!
    @brief    Single Argument constructor for GLModel.

    @param[in]    mode
        The primitive type of the model.
    *//*****************************************************************************/
    GLModel(GLenum mode);

    /*-----------------------------------------------------------------------------*/
    /* Member Functions                                                            */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Reads mesh data from a file.

    @param[in]    modelName
        The name of the model to initialise
    *//*****************************************************************************/
    void Init(const std::string& modelName);

    /****************************************************************************//*!
    @brief     Returns buffers back to GPU.
    *//*****************************************************************************/
    void Release();

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Functions                                                            */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Function to send VBO data containing position vertices ONLY to the
              GPU. Positions must be in 2D NDC.

              If there is more than 1 vertex attribute per model, DO NOT USE THIS!

    @param[in]    posData
        The vector containing the indices for drawing elements.
    *//*****************************************************************************/
    void SetVBOPoints(std::vector<glm::vec2> posData);

    /****************************************************************************//*!
    @brief    Templated function to bind an EBO with a VAO.

    @param[in]    indexData
        The vector containing the indices for drawing elements.
    *//*****************************************************************************/
    template <typename t>
    void SetEBO(std::vector<t> indexData);

    /****************************************************************************//*!
    @brief    Function to bind 2D points stored in the VBO to the VAO.
              Stored in attribute index 0. Binded using binding point 0.
    *//*****************************************************************************/
    void BindVBOPoints();

    /****************************************************************************//*!
    @brief    Function to bind colour values stored in the VBO to the VAO.
              Stored in attribute index 1. Binded using binding point 1.

              This function expects BindVBOPoints to be called first.

    @param[in]    offset
        The size of any previous vertex data preceding this.
    *//*****************************************************************************/
    void BindVBOColours(const GLsizeiptr& offset);
};

/*-------------------------------------------------------------------------------------*/
/* Templated Member Functions Definitions                                           */
/*-------------------------------------------------------------------------------------*/

template <typename t>
void GLModel::SetEBO(std::vector<t> indexData)
{
    glNamedBufferStorage(EBO, sizeof(t) * indexData.size(),
                         reinterpret_cast<GLvoid*>(indexData.data()), 
                         GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayElementBuffer(VAO, EBO);
}

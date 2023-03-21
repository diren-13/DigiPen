/************************************************************************************//*!
\file           gltexture.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2021
\brief          This file contains the declaration of class GLTexture.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <string>

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>

class GLTexture
{
  public:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    GLuint texObj;      // Handle to the texture object

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    GLuint width;       // The width of the texture image
    GLuint height;      // The height of an texture image
    GLuint texelSize;   // The size of each texel (in bytes)

  public:
    /*-----------------------------------------------------------------------------*/
    /* Constructors                                                                */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Default Constructor for GLTexture.

    @param[in]    w
        The width of the texture image.
    @param[in]    h
        The height of an texture image
    @param[in]    size
        The size of each texel (in bytes).
    *//*****************************************************************************/
    GLTexture(const GLuint& w = 256, const GLuint& h = 256, const GLuint& size = 4);

    /*-----------------------------------------------------------------------------*/
    /* Member Functions                                                            */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Initialises GLTexture by reading data from a binary file.

    @param[in]    filePath
        The file path to a binary file containing data for the texture.
    *//*****************************************************************************/
    void Init(const std::string& filePath);

    /****************************************************************************//*!
    @brief    Frees texture memory used by the GPU.
    *//*****************************************************************************/
    void Release();
};
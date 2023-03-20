/************************************************************************************//*!
\file           texture.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2021
\brief          This file contains the declaration of class Texture.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>

#include <utility.h>

class Texture
{
  public:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    std::vector<Colour>     texColours;         // Stores the colours of the texture obj
    glm::vec3               diffuseColour;      // Diffuse colour of the texture. White by default.

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    GLuint  width;      // The width of the texture image
    GLuint  height;     // The height of an texture image
    GLuint  texelSize;  // The size of each texel (in bytes)

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
    Texture(const GLuint& w = 256, const GLuint& h = 256, const GLuint& size = 4);

    /*-----------------------------------------------------------------------------*/
    /* Setter/Getter Functions                                                     */
    /*-----------------------------------------------------------------------------*/    
    Colour      GetTextureColour            (GLuint S, GLuint T) const;
    glm::vec3   GetTextureColourNormalised  (GLuint S, GLuint T) const;

    inline GLint GetWidth() const { return width; }
    inline GLint GetHeight() const { return height; }

    /*-----------------------------------------------------------------------------*/
    /* Member Functions                                                            */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Initialises GLTexture by reading data from a binary file.

    @param[in]    fileName
        The file path to a binary file containing data for the texture.
    *//*****************************************************************************/
    void Init(const std::string& fileName);
};
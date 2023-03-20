/************************************************************************************//*!
\file           gltexture.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2021
\brief          This file contains the implementation of class GLModel.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <gltexture.h>

#include <iostream>
#include <fstream>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

GLTexture::GLTexture(const GLuint& w, const GLuint& h, const GLuint& size)
: width		(w)
, height	(h)
, texelSize	(size)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &texObj);
}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions                                                             */
/*-------------------------------------------------------------------------------------*/
void GLTexture::Init(const std::string& filePath)
{
    std::ifstream binaryFile (filePath, std::ifstream::binary);

    if (!binaryFile)
    {
        std::cout << "Failed to open file: " << filePath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    binaryFile.seekg(0, binaryFile.end);
    int length = static_cast<int>(binaryFile.tellg());
    binaryFile.seekg(0, binaryFile.beg);

    // Allocate space for buffer
    char* texelPtr = new char[length * texelSize];
    // Read file as a block
    binaryFile.read(texelPtr, length);

    binaryFile.close();

    // Send data to GPU memory
    glTextureStorage2D(texObj, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(texObj, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texelPtr);

    delete[] texelPtr;
}

void GLTexture::Release()
{
    glDeleteTextures(1, &texObj);
}
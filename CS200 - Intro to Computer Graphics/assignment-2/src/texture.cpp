/************************************************************************************//*!
\file           texture.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 14, 2021
\brief          This file contains the implementation of class Texture.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <texture.h>

#include <iostream>
#include <iomanip>
#include <fstream>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

Texture::Texture(const GLuint& w, const GLuint& h, const GLuint& size)
: diffuseColour (1.f, 1.f, 1.f)
, width		    (w)
, height	    (h)
, texelSize	    (size)
{}


/*-------------------------------------------------------------------------------------*/
/* Setter/Getter Functions                                                             */
/*-------------------------------------------------------------------------------------*/

Colour Texture::GetTextureColour(GLuint S, GLuint T) const
{
    return *(texColours.data() + (T * width + S));
}

glm::vec3 Texture::GetTextureColourNormalised(GLuint S, GLuint T) const
{
    Colour textureColour    = *(texColours.data() + (T * width + S));
    const float INV_COLOUR  = 1.f / 255.f;

    glm::vec3 normalisedColour =
    {
        textureColour.r * INV_COLOUR,
        textureColour.g * INV_COLOUR,
        textureColour.b * INV_COLOUR,
    };

    return normalisedColour;
}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions                                                             */
/*-------------------------------------------------------------------------------------*/
void Texture::Init(const std::string& fileName)
{
    const std::string filePath = "../images/" + fileName;

    std::ifstream binaryFile (filePath, std::ifstream::binary);

    if (!binaryFile)
    {
        std::cout << "Failed to open file: " << filePath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    #ifdef _DEBUG
    std::cout << "File: " << filePath << " opened!" << std::endl;
    #endif

    // Redundant Step: 
    binaryFile.seekg(0, binaryFile.beg);

    // Read file as a block
    binaryFile.read(reinterpret_cast<char*>(&width),        sizeof(GLuint));
    binaryFile.read(reinterpret_cast<char*>(&height),       sizeof(GLuint));
    binaryFile.read(reinterpret_cast<char*>(&texelSize),    sizeof(GLuint));

    GLuint remainingFileSize = width * height * texelSize;

    std::vector<GLubyte> tmpTexObj;
    tmpTexObj.resize(remainingFileSize);
    binaryFile.read(reinterpret_cast<char*>(tmpTexObj.data()), remainingFileSize);

    #ifdef _DEBUG
    std::cout << "Closing File: " << filePath << " ..." << std::endl;
    #endif

    #ifndef _DEBUG
    std::string textureName = fileName.substr(0, fileName.find_first_of('.', 0));

    std::cout   << "Loaded Image: " << textureName                              << std::endl
                << "Image Width:  " << std::setw(5) << std::right << width      << std::endl
                << "Image Height: " << std::setw(5) << std::right << height     << std::endl
                << "Image BPP:    " << std::setw(5) << std::right << texelSize  << std::endl
                << std::endl;
    #endif
    binaryFile.close();

    // Swap red and blue
    for (size_t i = 0; i < tmpTexObj.size(); i += 3)
    {
        GLubyte red     = tmpTexObj[i];
        GLubyte green   = tmpTexObj[i + 1];
        GLubyte blue    = tmpTexObj[i + 2];

        texColours.emplace_back(red, green, blue);
    }
}
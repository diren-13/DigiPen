/************************************************************************************//*!
\file           glmodel.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 13, 2021
\brief          This file contains the implementation of class Model to be used with 
				GLApp.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <glmodel.h>

#include <iostream>
#include <sstream>
#include <array>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

GLModel::GLModel()
: primitiveType (0)
, drawCount     (0)
, primitiveCount(0)
{
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);
}

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
/* Member Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

void GLModel::Init(const std::string& modelName)
{
    std::string modelFilepath = "../meshes/" + modelName + ".msh";

    std::ifstream ifs(modelFilepath, std::ios::in);
    // Handle error case
    if (!ifs)
    {
        std::cout << "ERROR! Unable to open: " << modelFilepath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string             line;       // Holds the contents of a line in the file
    char                    prefix;     // Takes in the prefix of the line
    std::vector<glm::vec2>  posVtx;     // Container for the position vertices
    std::vector<GLushort>   idxVtx;     // Container for the vertex indices


    // Set file cursor to beginning of file
    ifs.seekg(0, std::ios::beg);
    // Skip the first line
    std::getline(ifs, line);

    while (!ifs.eof())
    {
        std::getline(ifs, line);
        std::istringstream lineData(line);
        lineData >> prefix;

        if (prefix == 'v')
        {
            glm::vec2 pos;
            lineData >> pos.x >> pos.y;
            posVtx.emplace_back(pos);
        }
        else
        {
            switch (prefix)
            {
                case 't':
                {
                    if (primitiveType == 0) { primitiveType = GL_TRIANGLES; }
                    break;
                }
                case 'f':
                {
                    if (primitiveType == 0) { primitiveType = GL_TRIANGLE_FAN; }
                    break;
                }
            }

            GLushort idx;
            while (lineData >> idx) 
            { 
                idxVtx.emplace_back(idx);
                ++drawCount;
            }
        }
    }

    ifs.close();

    SetVBOPoints(posVtx);
    SetEBO(idxVtx);
    glBindVertexArray(0);
}

void GLModel::Release()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void GLModel::SetVBOPoints(std::vector<glm::vec2> posData)
{
    glNamedBufferStorage(VBO, sizeof(glm::vec2) * posData.size(),
        reinterpret_cast<GLvoid*>(posData.data()),
        GL_DYNAMIC_STORAGE_BIT);

    BindVBOPoints();
}

void GLModel::BindVBOPoints()
{
    glEnableVertexArrayAttrib(VAO, POS_ATTRIB);
    glVertexArrayVertexBuffer(VAO, POS_BINDING_PT, VBO, 0, sizeof(glm::vec2));
    glVertexArrayAttribFormat(VAO, POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, POS_ATTRIB, POS_BINDING_PT);
}

void GLModel::BindVBOColours(const GLsizeiptr& offset)
{
    glEnableVertexArrayAttrib(VAO, CLR_ATTRIB);
    glVertexArrayVertexBuffer(VAO, CLR_BINDING_PT, VBO, offset, sizeof(glm::vec3));
    glVertexArrayAttribFormat(VAO, CLR_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(VAO, CLR_ATTRIB, CLR_BINDING_PT);
}

/************************************************************************************//*!
\file           glapp.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 13, 2021
\brief          This file contains the declaration of namespace GLApp that encapsulates 
                the functionality required to implement an OpenGL application including 
                compiling, linking, and validating shader programs
                setting up geometry and index buffers, 
                configuring VAO to present the buffered geometry and index data to
                vertex shaders, configuring textures (in later labs),
                configuring cameras (in later labs) and transformations (in later labs).

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

/*                                                                               guard
-------------------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H

/*                                                                            includes
-------------------------------------------------------------------------------------- */
#include <utility>          // std::pair

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>

#include <glhelper.h>       
#include <glslshader.h>    
#include <glmodel.h> 
#include <gltexture.h>
#include <glutility.h>

/************************************************************************************//*!
@brief    Encapsulates the behaviour of the application.
*//*************************************************************************************/
struct GLApp 
{
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    // -- Singletons
    static std::vector<GLModel>     models;             // Container of Models used for the tutorial
    static std::vector<GLTexture>   textures;           // Conatainer of Textures used for the tutorial
    static std::vector<GLSLShader>  shaderPrograms;     // Container of Shaders used for the tutorial
    static Controller               controller;         // Controller for tutorial 5 tasks and modes

    /*---------------------------------------------------------------------------------*/
    /* Lifecycle Functions                                                             */
    /*---------------------------------------------------------------------------------*/
    static void init();
    static void update(double delta_time);
    static void draw();
    static void cleanup();

    /*---------------------------------------------------------------------------------*/
    /* Member Functions															       */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief     Function to insert a shader program into the static container of
               shader programs.

    @param[in]    vertexShader
        The vertex shader source code for the shader program.
    @param[in]    fragmentShader
        The fragment shader source code for the shader program.
    *//*****************************************************************************/
    static void InsertShader(const std::string& vertexShader, const std::string& fragmentShader);
};



#endif /* GLAPP_H */
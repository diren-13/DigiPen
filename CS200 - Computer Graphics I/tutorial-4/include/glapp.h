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
#include <globject.h>
#include <camera2D.h>


/************************************************************************************//*!
@brief    Encapsulates the behaviour of the application.
*//*************************************************************************************/
struct GLApp 
{
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    // -- Singletons
    static std::map<std::string, GLModel>       models;         // Container for all instanced models in the app
    static std::map<std::string, GLSLShader>    shaderPrograms; // Container for all the shader programs in the app
    static std::map<std::string, GLObject>      objects;        // Container for all the objects in the app
    static Camera2D                             camera;
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
    @brief     Function to initialise the scene.

    @param[in]    sceneFilepath
        The file path containing the data for the scene.
    *//*****************************************************************************/
    static void InitScene(std::string sceneFilepath);

    /****************************************************************************//*!
    @brief     Function to insert a shader program into the static container of
               shader programs.

    @param[in]    programHandle
        The name of the program handle.
    @param[in]    vertexShader
        The vertex shader source code for the shader program.
    @param[in]    fragmentShader
        The fragment shader source code for the shader program.
    *//*****************************************************************************/
    static void InsertShaderProgram(const std::string& programHandle, 
                                    const std::string& vertexShader,
                                    const std::string& fragmentShader);

    /****************************************************************************//*!
    @brief     Function to insert a shader program into the static container of
               shader programs.

    @param[in]    modelName
        The name of the model to be initialised and inserted
    *//*****************************************************************************/
    static void InsertModel(const std::string& modelName);
};

#endif /* GLAPP_H */

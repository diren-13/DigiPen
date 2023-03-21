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

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glhelper.h>
#include <glslshader.h>


/************************************************************************************//*!
@brief    Encapsulates a colour to interface with openGL contexts.
*//*************************************************************************************/
struct Colour
{
    GLfloat R;	// Red
    GLfloat G;	// Green
    GLfloat B;	// Blue
    GLfloat A;	// Alpha
};

/************************************************************************************//*!
@brief    Encapsulates the behaviour of the application.
*//*************************************************************************************/
struct GLApp 
{
    /********************************************************************************//*!
    @brief    Encapsulates the behaviour of the application.
    *//*********************************************************************************/
    struct GLModel
    {
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        GLenum		primitiveType = 0;	// The primitive type to be rendered
        GLSLShader	shaderProg;		    // A shader program for the model
        GLuint		vaoid = 0;			// Handle for VAO
        GLuint		idxElemCnt = 0;		// Num of elements of primitive type to be rendered

        /*-----------------------------------------------------------------------------*/
        /* Member Functions                                                            */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief     Function to set up the Vertex Array Object and bind it to the current
                   openGL context.
        *//*****************************************************************************/
        void Setup_VAO();
        /****************************************************************************//*!
        @brief     Function to set up the Shader Program to be used.
        *//*****************************************************************************/
        void Setup_ShaderProgram();
        /****************************************************************************//*!
        @brief     Function to use shader program(s) and render the geometry.
        *//*****************************************************************************/
        void Draw();
    };

    /*---------------------------------------------------------------------------------*/
    /* Data Members																	   */
    /*---------------------------------------------------------------------------------*/
    static GLModel mdl;

    /*---------------------------------------------------------------------------------*/
    /* Lifecycle Functions                                                             */
    /*---------------------------------------------------------------------------------*/
    static void init();
    static void update(double delta_time);
    static void draw();
    static void cleanup();
};

#endif /* GLAPP_H */

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
#include <vector>

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
   @brief    Encapsulates data for a viewport.
   *//*********************************************************************************/
    struct GLViewport
    {
        /****************************************************************************//*!
        @brief     Constructor for a Viewport.

        @param[in]    _x
             The x-origin of the viewport.
        @param[in]    _y
             The y-origin of the viewport.
        @param[in]    _w
             The width of the viewport.
        @param[in]    _h
             The height of the viewport.
        *//*****************************************************************************/
        GLViewport(GLint _x, GLint _y, GLsizei _w, GLsizei _h)
        : x     (_x)
        , y     (_y)
        , width (_w)
        , height(_h)
        {}

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        GLint   x;          // x-origin of the viewport
        GLint   y;          // y-origin of the viewport
        GLsizei width;      // width of the viewport
        GLsizei height;     // height of the viewport
    };


    /********************************************************************************//*!
    @brief    Encapsulates the behaviour of the application.
    *//*********************************************************************************/
    struct GLModel
    {
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        GLenum		primitiveType = 0;	// The primitive type to be rendered
        GLuint		vaoid = 0;			// Handle for VAO
        
        GLuint      drawCount = 0;        // Num of vertices of a primitive type to be rendered
        GLuint      primitiveCount = 0;   // Num of primitives in an instance of the struct.

        GLSLShader	shaderProgram;       // A shader program for the model

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief     Function to set up a Shader Program to be used.

        @param[in]    vertShader
            The contents of the vertex shader used for compilation
        @param[in]    fragShader
            The contents of the fragment shader used for compilation
        *//*****************************************************************************/
        void Setup_ShaderProgram(std::string vertShader, std::string fragShader);
        /****************************************************************************//*!
        @brief     Function to use shader program(s) and render the geometry.
        *//*****************************************************************************/
        void Draw();

    };

    /*---------------------------------------------------------------------------------*/
    /* Data Members																	   */
    /*---------------------------------------------------------------------------------*/
    static std::vector<GLViewport> vps;     // Container for multiple viewports
    static std::vector<GLModel> models;     // Container for all the models in the app

    /*---------------------------------------------------------------------------------*/
    /* Lifecycle Functions                                                             */
    /*---------------------------------------------------------------------------------*/
    static void init();
    static void update(double delta_time);
    static void draw();
    static void cleanup();

    /*---------------------------------------------------------------------------------*/
    /* Function Members																   */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief     Function to render slices + 1 points for a horizontal span between 
               NDC (-1,0) and (1,0)

    @param[in]    slices
        The number of points to be rendered in the x-axis
    @param[in]    stacks
        The number of points to be rendered in the y-axis
    @param[in]    vertShader
        The contents of the vertex shader used for compilation
    @param[in]    fragShader
        The contents of the fragment shader used for compilation
    *//*****************************************************************************/
    static GLModel pointsModel(int slices, int stacks, std::string vertShader, std::string fragShader);
    
    /****************************************************************************//*!
    @brief     Function to render a hex pattern made of lines across the viewport

    @param[in]    slices
        The number of columns to be rendered
    @param[in]    stacks
        The number of rows to be rendered
    @param[in]    vertShader
        The contents of the vertex shader used for compilation
    @param[in]    fragShader
        The contents of the fragment shader used for compilation
    *//*****************************************************************************/
    static GLModel linesModel(int slices, int stacks, std::string vertShader, std::string fragShader);

    /****************************************************************************//*!
    @brief     Function to render a disk using triangle fans as the primitive type.

    @param[in]    slices
        The number of triangles in the fan
    @param[in]    vertShader
        The contents of the vertex shader used for compilation
    @param[in]    fragShader
        The contents of the fragment shader used for compilation
    *//*****************************************************************************/
    static GLModel trifanModel(int slices, std::string vertShader, std::string fragShader);

    /****************************************************************************//*!
    @brief     Function to render triangle strips

    @param[in]    slices
        The number of points to be rendered in the x-axis
    @param[in]    stacks
        The number of points to be rendered in the y-axis
    @param[in]    vertShader
        The contents of the vertex shader used for compilation
    @param[in]    fragShader
        The contents of the fragment shader used for compilation
    *//*****************************************************************************/
    static GLModel tristripModel(int slices, int stacks, std::string vertShader, std::string fragShader);

    /*---------------------------------------------------------------------------------*/
    /* Helper Functions																   */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief     Function to bind 2D points stored in the VBO to the VAO.
               Stored in attribute index 0. Binded using binding point 0.

    @param[in]    vaobj
        The VAO to bind to.
    @param[in]    buffer
        The vbo holding the vertex data.
    *//*****************************************************************************/
    static void BindVBOPoints(GLuint& vaobj, GLuint& buffer);

    /****************************************************************************//*!
    @brief     Function to bind 3D colour values stored in the VBO to the VAO.
               Stored in attribute index 1. Binded using binding point 1.

    @param[in]    vaobj
        The VAO to bind to.
    @param[in]    buffer
        The vbo holding the vertex data.
    *//*****************************************************************************/
    static void BindVBOColour(GLuint& vaobj, GLuint& buffer, const GLsizeiptr& offset);

};

#endif /* GLAPP_H */

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
#include <vector>
#include <array>
#include <utility>	        // std::pair
#include <list>             // std::list (doubly-linked list)

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glhelper.h>
#include <glslshader.h>


/************************************************************************************//*!
@brief    Encapsulates the behaviour of the application.
*//*************************************************************************************/
struct GLApp 
{
    using VPSS = std::vector<std::pair<std::string, std::string>>;

    /********************************************************************************//*!
    @brief    Encapsulates a model made up of geometric primitives.
    *//*********************************************************************************/
    struct GLModel
    {
        /*-----------------------------------------------------------------------------*/
        /* Member Variables                                                            */
        /*-----------------------------------------------------------------------------*/
        GLenum	primitiveType;	    // The primitive type to be rendered
        GLuint  drawCount;          // Num of vertices of a primitive type to be rendered
        GLuint  primitiveCount;     // Num of primitives in an instance of the struct  
        GLuint  VAO;                // Handle to the Model's VAO
        GLuint  VBO;                // Handle to the Model's VBO
        GLuint  EBO;                // Handle to the Model's EBO

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief    Default constructor for GLModel.
        *//*****************************************************************************/
        GLModel()
        : primitiveType(0)
        , drawCount(0)
        , primitiveCount(0)
        {
            glCreateVertexArrays(1, &VAO);
            glCreateBuffers(1, &VBO);
            glCreateBuffers(1, &EBO);
        }

        /****************************************************************************//*!
        @brief    Single Argument constructor for GLModel.

        @param[in]    mode
            The primitive type of the model.
        *//*****************************************************************************/
        GLModel(GLenum mode)
        : primitiveType(mode)
        , drawCount(0)
        , primitiveCount(0)
        {
            glCreateVertexArrays(1, &VAO);
            glCreateBuffers(1, &VBO);
            glCreateBuffers(1, &EBO);
        }

        /*-----------------------------------------------------------------------------*/
        /* Member Functions                                                            */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief    Deletes the VAO, VBO and EBO of the model.
        *//*****************************************************************************/
        void deleteHandles();
    };

    /********************************************************************************//*!
    @brief    Encapsulates a state required to update and render an instance of a
              GLModel.
    *//*********************************************************************************/
    struct GLObject
    {
        /*-----------------------------------------------------------------------------*/
        /* Member Variables                                                            */
        /*-----------------------------------------------------------------------------*/
        glm::vec2   position;       // translation vector
        glm::vec2   scale;          // scaling vector
        GLfloat     rotation;       // Current abs orientation angle
        GLfloat     angularSpeed;   // Rate of change of rotation. Measured in angle/s
        glm::mat3   modelToNDC;     // Homogenous matrix to compute the transform for the Model coordinates to NDC coordinates
        GLuint      modelRef;       // Reference to an instanced model.
        GLuint      shaderRef;      // Reference to a shader program to be used to for the model.

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief    Default constructor for GLObject.
        *//*****************************************************************************/
        GLObject()
        : position      ()                  // invokes default ctor for glm::vec2
        , scale         ()                  // invokes default ctor for glm::vec2
        , rotation      (0.f)
        , angularSpeed  (0.f)
        , modelToNDC    (glm::mat3(1.f))    // sets modelToNDC to identity mtx by default
        , modelRef      (0)
        , shaderRef     (0)
        {}        
        
        /*-----------------------------------------------------------------------------*/
        /* Member Functions															   */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief    Initialises the object state.
        *//*****************************************************************************/
        void init();

        /****************************************************************************//*!
        @brief    Updates the object's model transformation matrix.

        @param[in]    dt
            The delta-time between each frame.
        *//*****************************************************************************/
        void update(GLdouble dt);

        /****************************************************************************//*!
        @brief    Render object's model.
        *//*****************************************************************************/
        void draw() const;
    };

    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    // -- Singletons
    static std::vector<GLModel>     models;         // Container for all instanced models in the app
    static std::vector<GLSLShader>  shaderPrograms; // Container for all the shader programs in the app
    static std::list<GLObject>      objects;        // Container for all the objects in the app

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
    @brief     Function to initialised a container to hold the models.
    *//*****************************************************************************/
    static void initModelsCont();

    /****************************************************************************//*!
    @brief     Function to initialised the a container to hold the models.

    @param[in]    vpss
        A vector of paired strings. The pair should contain a vertex shader and a 
        fragment shader.
    *//*****************************************************************************/
    static void initShaderProgramsCont(VPSS const& vpss);

    /****************************************************************************//*!
    @brief     Function to render a box with randomised colour vertices.
    *//*****************************************************************************/
    static GLModel boxModel();

    /****************************************************************************//*!
    @brief     Function to render a heart with randomised colour vertices.
               X-coordinates: 16sin(t)
               Y-coordinates: 13cos(t)-5cos(2t)-2cos(3t)-cos(4t)
               Where t is the angle in radians

    @param[in]    slices
        The number of slices for the mystery model.
    *//*****************************************************************************/
    static GLModel mysteryModel(const int& slices);

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

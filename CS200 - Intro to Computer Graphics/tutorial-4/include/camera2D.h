/************************************************************************************//*!
\file           camera2D.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 4, 2021
\brief          This file contains the declaration of class Camera2D to be used with 
                GLApp.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <utility>

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <globject.h>

/********************************************************************************//*!
@brief    Encapsulates a 2D camera object to be used to interact with the world
          in a scene.
*//*********************************************************************************/
class Camera2D
{
  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    GLObject*               cameraObj;          // Pointer to an object that embeds the camera
    glm::vec2               up;                 // The up facing vector of the camera
    glm::vec2               right;              // The right facing vector of the camera
    GLint                   height;             // The current zoom level of the camera
    GLfloat                 aspectRatio;        // The fixed aspect ratio of the camera
    // Matrices
    glm::mat3               worldToCamera;      // The viewport transform matrix of the camera
    glm::mat3               cameraToNDC;        // The camera window to NDC transform matrix
    glm::mat3               worldToNDC;         // THe world coordinates to NDC transform matrix

  public:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    // Controlling Variables
    std::pair<GLint, GLint> heightInterval;     // The zoom interval
    GLint                   heightChangeDir;    // Increasing 1, Decreasing -1
    GLint                   heightChangeVal;    // Increments for zoom
    GLfloat                 linearSpeed;        // Speed when moving along up-axis
    // Camera flags
    GLboolean               camType;        // V
    GLboolean               zooming;        // Z
    GLboolean               turningLeft;    // H
    GLboolean               turningRight;   // K
    GLboolean               moving;         // U

  public:
    /*-----------------------------------------------------------------------------*/
    /* Constructors                                                                */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Default constructor for Camera2D. The aspect ratio of the camera must
              match the aspect ratio of the viewport.
    *//*****************************************************************************/
    Camera2D();

    /*-----------------------------------------------------------------------------*/
    /* Getters														               */
    /*-----------------------------------------------------------------------------*/
    inline const GLObject*      GetCameraObject()   { return cameraObj; }
    inline const GLint&         GetHeight()         { return height; }
    inline const GLfloat&       GetAspectRatio()    { return aspectRatio; }
    inline const glm::mat3&     GetWorldToCamera()  { return worldToCamera; }
    inline const glm::mat3&     GetCameraToNDC()    { return cameraToNDC; }
    inline const glm::mat3&     GetWorldToNDC()     { return worldToNDC; }

    /*-----------------------------------------------------------------------------*/
    /* Setters														               */
    /*-----------------------------------------------------------------------------*/
    inline void SetHeight(const GLint& h)           { height = h; }
    inline void SetAspectRatio(const GLfloat& ar)   { aspectRatio = ar; }

    /*-----------------------------------------------------------------------------*/
    /* Member Functions															   */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Initialises the state of a camera object.

    @param[in]        window
        Pointer to the context window.
    @param[in/out]    camera
        Pointer to the camera object.
    *//*****************************************************************************/
    void Init(GLFWwindow* window, GLObject* camera);

    /****************************************************************************//*!
    @brief    Updates the state of a camera object.

    @param[in]    window
        Pointer to the context window.
    @param[in]    dt
        The time between each frame.
    *//*****************************************************************************/
    void Update(GLFWwindow* window, GLdouble dt);

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Functions															   */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Computes the up and right vectors. A camera object must be present.
    *//*****************************************************************************/
    void ComputeVectors();

    /****************************************************************************//*!
    @brief    Computes the transformation matrices for the camera. 
              A camera object must be present.
    *//*****************************************************************************/
    void ComputeMatrices();
};
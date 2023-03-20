/************************************************************************************//*!
\file           globject.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 4, 2021
\brief          This file contains the declaration of class Object to be used with GLApp.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <map>
#include <string>
#include <utility>

#include <GL/glew.h>		// for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glmodel.h>
#include <glslshader.h>

class Camera2D;             // Forward declaration for camera2D

/********************************************************************************//*!
@brief    Encapsulates a state required to update and render an instance of a
          GLModel.
*//*********************************************************************************/
struct GLObject
{
  public:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    glm::vec2   position;       // translation vector
    glm::vec2   scale;          // scaling vector
    GLfloat     rotation;       // Current abs orientation angle
    GLfloat     rotationSpeed;  // Rate of change of rotation. Measured in angle/s
    glm::vec3   colour;         // The colour values of the model

    std::map<std::string, GLModel>::iterator    modelRef;
    std::map<std::string, GLSLShader>::iterator shaderRef;

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Variables                                                            */
    /*-----------------------------------------------------------------------------*/
    glm::mat3   modelTransform; // Homogenous matrix for transforming the Model coordinates
    glm::mat3   modelToNDC;     // Homogenous matrix for transforming the Model coordinates to NDC coordinates

  public:
    /*-----------------------------------------------------------------------------*/
    /* Constructors                                                                */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Default constructor for GLObject.
    *//*****************************************************************************/
    GLObject();

    /****************************************************************************//*!
    @brief    Constructor for GLObject.

    @param[in]    _pos
        The initial position of the object.
    @param[in]    _scale
        The initial scale of the object.
    @param[in]    _rpt
        The initial rotation of the object.
    @param[in]    _rotSpd
        The initial rotation speed of the object.
    @param[in]    _clr
        The initial colour of the object.
    *//*****************************************************************************/
    GLObject(const glm::vec2& _pos, const glm::vec2& _scale, const GLfloat& _rot, 
             const GLfloat& _rotSpeed, const glm::vec3& _clr);

    /*-----------------------------------------------------------------------------*/
    /* Member Functions															   */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Updates the object's model transformation matrix.

    @param[in]    dt
        The delta-time between each frame.
    @param[in]    camObj
        The delta-time between each frame.
    *//*****************************************************************************/
    void update(GLdouble dt, Camera2D* camObj);

    /****************************************************************************//*!
    @brief    Render object's model.
    *//*****************************************************************************/
    void draw() const;

  private:
    /*-----------------------------------------------------------------------------*/
    /* Member Functions															   */
    /*-----------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Wraps the rotation of the object to a specified range of (-r, r)
    *//*****************************************************************************/
    void wrapRotation(const GLfloat& range);
};

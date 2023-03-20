/************************************************************************************//*!
\file           globject.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 4, 2021
\brief          This file contains the declaration of class Object to be used with GLApp.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <numeric>

#include <globject.h>
#include <camera2D.h>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

GLObject::GLObject()
: position		()					// invokes default ctor for glm::vec2
, scale			()					// invokes default ctor for glm::vec2
, rotation		(0.f)
, rotationSpeed	(0.f)
, colour		()					// invokes default ctor for glm::vec3
, modelTransform(glm::mat3(1.f))	// sets modelTransform to identity mtx by default
, modelToNDC	(glm::mat3(1.f))	// sets modelToNDC to identity mtx by default
{}

GLObject::GLObject(const glm::vec2& _pos, const glm::vec2& _scale, const GLfloat& _rot,
                   const GLfloat& _rotSpeed, const glm::vec3& _clr)
: position		(_pos)				// invokes default ctor for glm::vec2
, scale			(_scale)			// invokes default ctor for glm::vec2
, rotation		(_rot)
, rotationSpeed	(_rotSpeed)
, colour		(_clr)				// invokes default ctor for glm::vec3
, modelTransform(glm::mat3(1.f))	// sets modelTransform to identity mtx by default
, modelToNDC	(glm::mat3(1.f))	// sets modelToNDC to identity mtx by default
{}

/*-------------------------------------------------------------------------------------*/
/* Member Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

void GLObject::update(GLdouble dt, Camera2D* camObj)
{
    // Wrap rotation within the range of (-360, 360)
    wrapRotation(360.f);

    // NOTE: transpose matrix from row major to column major to fit GLSL uniforms
    float rotationRad = glm::radians(rotation);
    
    if (this != camObj->GetCameraObject())
    {
        rotation += static_cast<GLfloat>(rotationSpeed * dt);
        rotationRad = glm::radians(rotation);
    }

    // TRS Matrix (Row Major)
    modelTransform =
    {
        scale.x * cos(rotationRad),     scale.x * sin(rotationRad),     0.f,
        scale.y * -sin(rotationRad),    scale.y * cos(rotationRad),     0.f,
        position.x,                     position.y,                     1.f
    };

    modelToNDC = camObj->GetWorldToNDC() * modelTransform;
}

void GLObject::draw() const
{
    GLModel    model           = modelRef->second;
    GLSLShader shaderProgram   = shaderRef->second;

    // Setup
    shaderProgram.Use();            // install shader
    glBindVertexArray(model.VAO);   // bind VAO

    // Copy uniform 3x3 modelToNDC matrix to vertex shader
    shaderProgram.SetUniform("uModel_to_NDC", modelToNDC);

    // Copy uniform colour to fragment shader
    shaderProgram.SetUniform("uColour", colour);

    glDrawElements(model.primitiveType, model.drawCount, GL_UNSIGNED_SHORT, NULL);

    // Clean Up
    glBindVertexArray(0);       // unbind VAO
    shaderProgram.UnUse();      // uninstall shader
}

/*-------------------------------------------------------------------------------------*/
/* Private Member Functions                                                            */
/*-------------------------------------------------------------------------------------*/

void GLObject::wrapRotation(const GLfloat& range)
{
    if (rotation >= range || rotation <= -range) { rotation = 0.f; }
}
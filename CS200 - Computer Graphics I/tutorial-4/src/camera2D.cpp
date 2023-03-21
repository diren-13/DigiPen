/************************************************************************************//*!
\file           camera2D.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 4, 2021
\brief          This file contains the implementation of class Camera2D to be used with 
                GLApp.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <camera2D.h>

#include <glhelper.h>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

Camera2D::Camera2D()
: cameraObj		    (nullptr)
, up			    (0.f, 1.f)			// Sets the up vector to <0,1>
, right			    (1.f, 0.f)			// Sets the right vector to <1,0>
, height		    (1000)				// the default height of the camera
, aspectRatio	    (static_cast<float>(GLHelper::width) / GLHelper::height)
// Controlling Variables
, heightInterval    (500, 2000)           
, heightChangeDir   (1)
, heightChangeVal   (5)
, linearSpeed       (2.f)
//Camera Flags
, camType           (GL_FALSE)
, zooming           (GL_FALSE)
, turningLeft       (GL_FALSE)
, turningRight      (GL_FALSE)
, moving            (GL_FALSE)
// Matrices
, worldToCamera	    (glm::mat3(1.f))	// Sets the view transform to an identity matrix
, cameraToNDC	    (glm::mat3(1.f))	// Sets the CameraToNDC transform to an identity matrix
, worldToNDC	    (glm::mat3(1.f))	// Sets the WorldToNDC transform to an identity matrix
{}

/*-------------------------------------------------------------------------------------*/
/* Member Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

void Camera2D::Init(GLFWwindow* window, GLObject* camera)
{
    // Assign pointer if empty
    if (cameraObj == nullptr) { cameraObj = camera; }

    ComputeVectors();
    ComputeMatrices();
}

void Camera2D::Update(GLFWwindow* window, GLdouble dt)
{
    // Check for input
    turningLeft     = glfwGetKey(window, GLFW_KEY_H);
    turningRight    = glfwGetKey(window, GLFW_KEY_K);
    moving          = glfwGetKey(window, GLFW_KEY_U);
    zooming         = glfwGetKey(window, GLFW_KEY_Z);
    
    // -- Change camera state
    if (GLHelper::keystateV == GL_TRUE)
    { 
        camType = !camType;
        GLHelper::keystateV = GL_FALSE;
    }

    // -- Update rotation
    if (turningLeft)
    {
        cameraObj->rotation += cameraObj->rotationSpeed;
        turningLeft = false;
    }
    if (turningRight)
    {
        cameraObj->rotation -= cameraObj->rotationSpeed;
        turningRight = false;
    }
    // -- Update up and right vectors
    ComputeVectors();

    // -- Update position 
    if (moving)
    {
        cameraObj->position += (linearSpeed * up);
        moving = false;
    }

    // -- Update zoom
    if (zooming)
    {
        switch (heightChangeDir)
        {
            case 1:     // Zooming Out
            {
                height += heightChangeVal;
                break;
            }
            case -1:    // Zooming In
            {
                height -= heightChangeVal;
                break;
            }
        }

        if (height >= heightInterval.second)
        {
            heightChangeDir = -1;
        }
        else if (height <= heightInterval.first)
        {
            heightChangeDir = 1;
        }

        zooming = false;
    }

    // Recalculte aspect ratio if window was resized
    int frameBufferWidth    = 0;
    int frameBufferHeight   = 0;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

    aspectRatio = static_cast<float>(frameBufferWidth) / frameBufferHeight;
    
    // Compute all camera matrices
    ComputeMatrices();

    // Update object
    cameraObj->update(dt, this);
}

/*-------------------------------------------------------------------------------------*/
/* Private Member Functions                                                            */
/*-------------------------------------------------------------------------------------*/

void Camera2D::ComputeVectors()
{
    assert(cameraObj != nullptr);

    // Compute up and right vectors
    float rotationRad = glm::radians(cameraObj->rotation);

    up		= glm::vec2{ -sin(rotationRad), cos(rotationRad) };
    right	= glm::vec2{ cos(rotationRad), sin(rotationRad) };
}

void Camera2D::ComputeMatrices()
{
    assert (cameraObj != nullptr);

    glm::vec2	worldPos	= cameraObj->position;
    GLfloat		width		= aspectRatio * height;

    switch (camType)
    {
        case GL_FALSE:  // Free camera
        {
            worldToCamera =
            {
                1.f,			0.f,			0.f,
                0.f,			1.f,			0.f,
                -worldPos.x,	-worldPos.y,	1.f
            };
            break;
        }
        case GL_TRUE:   // First-person camera
        {
            worldToCamera =
            {
                right.x,					up.x,						0.f,
                right.y,					up.y,						0.f,
                glm::dot(-right, worldPos), glm::dot(-up, worldPos),	1.f
            };
        }
    }

    // Cam->Win->NDC Matrix
    cameraToNDC =
    {
        2.f / width,	0.f,			0.f,
        0.f,			2.f / height,	0.f,
        0.f,			0.f,			1.f
    };

    // World to NDC = CamToNDC x WorldToCam
    worldToNDC = cameraToNDC * worldToCamera;
}
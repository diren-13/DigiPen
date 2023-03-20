/************************************************************************************//*!
\file           glutility.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 11, 2021
\brief          This file contains the implementations of utility functions and a
                controller class specific to controlling the interaction with tutorial 5.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <utility.h>

#include <iostream>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

// Controller
// --------------------------------------------------------------------------------------
Controller::Controller()
: model         (0)
, renderMode    (0)
, rotateModel   (false)
, rotateLight   (false)
, numModels     (0)
, numModes      (0)
{}

// Color
// --------------------------------------------------------------------------------------
Colour::Colour(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
: r (_r)
, g (_g)
, b (_b)
, a (_a)
{}

Colour::Colour(const glm::vec3& colour, GLubyte _a)
: r (static_cast<GLubyte>(colour.r * 255))
, g (static_cast<GLubyte>(colour.g * 255))
, b (static_cast<GLubyte>(colour.b * 255))
, a (_a)
{}

Colour::Colour(const glm::dvec3& colour, GLubyte _a)
: Colour(static_cast<glm::vec3>(colour), _a)
{};



/*-------------------------------------------------------------------------------------*/
/* Member Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

// Controller
// --------------------------------------------------------------------------------------
void Controller::Update(GLboolean& modelKey, GLboolean& renderModeKey, GLboolean& rotateModelKey, GLboolean& rotateLightKey)
{
   // Update model
   if (modelKey == GL_TRUE)
   {
       model == numModels ? model = 0 : ++model;
       modelKey = GL_FALSE;

       #ifdef _DEBUG
       std::cout << "Model: " << model << std::endl;
       #endif
   }

   // Update render mode
   if (renderModeKey == GL_TRUE)
   {
       renderMode == numModes ? renderMode = 0 : ++renderMode;
       renderModeKey = GL_FALSE;

       #ifdef _DEBUG
       std::cout << "Render Mode: " << renderMode << std::endl;
       #endif
   }

   // Update model rotation
   if (rotateModelKey == GL_TRUE)
   {
       rotateModel = !rotateModel;
       rotateModelKey = GL_FALSE;

       #ifdef _DEBUG
       std::cout << "Rotate Model: " << rotateModel << std::endl;
       #endif
   }

   // Update light rotation
   if (rotateLightKey == GL_TRUE)
   {
       rotateLight = !rotateLight;
       rotateLightKey = GL_FALSE;

       #ifdef _DEBUG
       std::cout << "Rotate Light: " << rotateLight << std::endl;
       #endif
   }
}




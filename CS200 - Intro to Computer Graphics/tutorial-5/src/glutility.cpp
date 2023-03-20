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

#include <glutility.h>

#include <iostream>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

Controller::Controller()
: task          (0)
, modulateMode  (false)
, blendMode     (false)
, numTasks      (0)
{}

Controller::Controller(const int& _numTasks)
: task          (0)
, modulateMode  (false)
, blendMode     (false)
, numTasks      (_numTasks)
{}

/*-------------------------------------------------------------------------------------*/
/* Member Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

void Controller::Update(GLboolean& taskKey, GLboolean& modulateModeKey, GLboolean& blendModeKey)
{
    // Change task
    if (taskKey == GL_TRUE)
    {
        task == numTasks ? task = 0 : ++task;
        taskKey = GL_FALSE;

        #ifdef _DEBUG
        std::cout << "Task: " << task << std::endl;
        #endif
    }

    // Change modulate mode
    if (modulateModeKey == GL_TRUE)
    {
        modulateMode = !modulateMode;
        modulateModeKey = GL_FALSE;

        #ifdef _DEBUG
        std::cout << "Modulate Mode: " << (modulateMode ? "TRUE" : "FALSE") << std::endl;
        #endif
    }

    // Change alpha blending mode
    if (blendModeKey == GL_TRUE)
    {
        blendMode = !blendMode;
        blendModeKey = GL_FALSE;

        #ifdef _DEBUG
        std::cout << "Alpha Blend Mode: " << (blendMode ? "TRUE" : "FALSE") << std::endl;
        #endif
    }
}
/************************************************************************************//*!
\file           glutility.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 11, 2021
\brief          This file contains the declarations of some utility functions.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <chrono>
#include <random>

#include <GLFW/glfw3.h>

/************************************************************************************//*!
@brief    Encapsulates a helper struct to control the behaviour of the application.
          Tailor-made to this tutorial.
*//*************************************************************************************/
struct Controller
{
  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    int         task;          // toggle between tasks
    bool        modulateMode;  // toggle between colour modulating mode
    bool        blendMode;     // toggle between alpha blending mode

  private:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    const int   numTasks;       // Number of tasks present in the scene for the tutorial

  public:
    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief     Default constructor for GLController.
    *//*****************************************************************************/
    Controller();

    /****************************************************************************//*!
    @brief     Constructor for GLController.

    @param[in]    _numTasks
        The number of tasks for the controller to keep track of
    *//*****************************************************************************/
    Controller(const int& _numTasks);

    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief     Updates the controller using keyboard inputs with GLFW inputs.

    @param[in]   taskKey
        The boolean for tracking they key state to change the task.
    @param[in]   modulateModeKey
        The boolean for tracking they key state to change the modulate mode.
    @param[in]   blendModeKey
        The boolean for tracking they key state to change the alpha blending mode.
    *//*****************************************************************************/
    void Update(GLboolean& taskKey, GLboolean& modulateModeKey, GLboolean& blendModeKey);
};

namespace GLUtility 
{
    /************************************************************************************//*!
    @brief     Gets a random float between a specified range with uniform distribution.
               Generates a number between 0.f and 1.f by defualt.

               Default template type is float.

    @param[in]    lowerBound
        The lower bound of the randomised range
    @param[in]    upperBound
        The upper bound of the randomised range
    *//*************************************************************************************/
    template <typename floatType = float>
    float randFloat(const floatType& lowerBound = 0.f, const floatType& upperBound = 1.f)
    {
        unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());

        std::default_random_engine generator(seed);

        std::uniform_real_distribution<floatType> distribution(lowerBound, upperBound);

        return distribution(generator);
    }

    /************************************************************************************//*!
    @brief     Gets a random int between a specified range. Generates a random number
               between 0 and 1 by default.

               Default template type is int.

    @param[in]    lowerBound
        The lower bound of the randomised range
    @param[in]    upperBound
        The upper bound of the randomised range
    *//*************************************************************************************/
    template <typename intType = int>
    int randInt(const intType& lowerBound = 0, const intType& upperBound = 1)
    {
        unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());

        std::default_random_engine generator(seed);

        std::uniform_int_distribution<intType> distribution(lowerBound, upperBound);

        return distribution(generator);
    }

} // namespace GLUtility
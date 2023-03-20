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
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

/************************************************************************************//*!
@brief    Encapsulates a helper struct to control the behaviour of the application.
          Tailor-made to this assignment.
*//*************************************************************************************/
struct Controller
{
  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    int     model;
    int     renderMode;
    bool    rotateModel;
    bool    rotateLight;

  private:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    int numModels;
    int numModes;

  public:
    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief      Default Constructor for GLController.
    *//*****************************************************************************/
    Controller();

    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    // Setters
    inline void SetNumModels(int modelCount)    { numModels = modelCount - 1; }
    inline void SetNumModes (int modeCount)     { numModes = modeCount - 1; }

    /****************************************************************************//*!
    @brief     Updates the controller using keyboard inputs with GLFW inputs.
    *//*****************************************************************************/
    void Update(GLboolean& modelKey, GLboolean& renderModeKey, GLboolean& rotateModelKey, GLboolean& rotateLightKey);
};


/************************************************************************************//*!
@brief    Encapsulates colour values in the form of RGBA.
*//*************************************************************************************/
union Colour
{
    struct
    {
        GLubyte r;
        GLubyte g;
        GLubyte b;
        GLubyte a;
    };

    GLubyte     val[4];     // treat <r,g,b,a> as array of 8-bit unsigned values
    GLuint      raw;        // treat <r,g,b,a> as 32-bit unsigned value

    /****************************************************************************//*!
    @brief    Default Constructor for Color.

    @param[in]  _r
        The red channel. Defaults to 0.
    @param[in]  _g
        The green channel. Defaults to 0.
    @param[in]  _b
        The blue channel. Defaults to 0.
    @param[in]  _a
        The alpha value. Defaults to 255.
    *//*****************************************************************************/
    Colour(GLubyte _r = 0, GLubyte _g = 0, GLubyte _b = 0, GLubyte _a = 255);

    /****************************************************************************//*!
    @brief    Conversion Constructor for Color.

    @param[in]  colour
        A colour stored as a vec3.
    @param[in]  _a
        The alpha value. Defaults to 255.
    *//*****************************************************************************/
    Colour(const glm::vec3& colour, GLubyte _a = 255);
    Colour(const glm::dvec3& colour, GLubyte _a = 255);
};


namespace Utility 
{
    const constexpr int TRI_PTS = 3;

    /****************************************************************************//*!
    @brief    Check's if a pixel is within the window's range

    @param[in]  x
        The x-coordinate of the pixel.
    @param[in]  y
        The y-coordinate of the pixel.
    *//*****************************************************************************/
    template <typename intType = int>
    bool Scissor(intType x, intType y, intType width, intType height)
    {
        return (x >= 0 && x < width && y >= 0 && y < height);
    }

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
        static std::default_random_engine generator;
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
        static std::default_random_engine generator;
        std::uniform_int_distribution<intType> distribution(lowerBound, upperBound);
        return distribution(generator);
    }

} // namespace Utility
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
    int     object;         // toggle between tasks
    int     renderMode;     // toggle between colour modulating mode
    bool    rotate;         // toggle between alpha blending mode

  private:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    int     numObjects;     // Number of objects in the scene for the assignment
    int     numRenderModes; // Number of render modes in the scene for the assignment

  public:
    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief     Constructor for GLController.

    @param[in]    _numObjects
        The number of tasks for the controller to keep track of
    *//*****************************************************************************/
    Controller(const int& _numObjects = 0);

    /*---------------------------------------------------------------------------------*/
    /* Setter Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    inline void SetNumObjects(int numObj)       { numObjects = numObj - 1; }
    inline void SetNumRenderModes(int numModes) { numRenderModes = numModes - 1; }

    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief     Updates the controller using keyboard inputs with GLFW inputs.

    @param[in]   objectKey
        The key for tracking the object to be rendered.
    @param[in]   renderModeKey
        The key for tracking the rendering mode of the scene.
    @param[in]   toggleRotateKey
        The key for toggling rotation of the object in the scene.
    *//*****************************************************************************/
    void Update(GLboolean& objectKey, GLboolean& renderModeKey, GLboolean& toggleRotateKey);
};


/************************************************************************************//*!
@brief    Encapsulates colour values in the form of RGBA.
*//*************************************************************************************/
union Color
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
    Color(GLubyte _r = 0, GLubyte _g = 0, GLubyte _b = 0, GLubyte _a = 255);

    /****************************************************************************//*!
    @brief    Constructor for Color.

    @param[in]  colour
        A colour stored as a vec3.
    @param[in]  _a
        The alpha value. Defaults to 255.
    *//*****************************************************************************/
    Color(const glm::vec3& colour, GLubyte _a = 255);
    Color(const glm::dvec3& colour, GLubyte _a = 255);
};

/************************************************************************************//*!
@brief    Encapsulates the implicit equation of an edge.
*//*************************************************************************************/
struct EdgeEquation
{
  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    glm::dvec2  normal;     // <a, b>
    double      c;          // (p0.x * p1.y) - (p1.x * p0.y)
    bool        topLeft;    

    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief    Constructor for EdgeEquation.

    @param[in]  startPoint
        The start point on the edge.
    @param[in]  endPoint
        The end point on the edge.
    *//*****************************************************************************/
    EdgeEquation(const glm::dvec2& startPoint, const glm::dvec2& endPoint);

    EdgeEquation() = delete;

    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief    Evaluates the edge equation in the form of ax + by + c

    @param[in]  point
        A point to evaluate the equation at.

    @returns    The evaluation of the equation at a given point.
    *//*****************************************************************************/
    double Evaluate(const glm::dvec2& point) const;

    /****************************************************************************//*!
    @brief    Checks if a point is inside or outside of an edge based on the 
              top-left tie-breaker rule.

    @param[in]  point
        A point to check against.

    @returns    True if the point is on the inside edge or on a top or left edge. 
                False if it is not.
    *//*****************************************************************************/
    bool PointInEdgeTopLeft(const glm::dvec2& point) const;
};

struct TriangleRasteriser
{
  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    float           signedArea;     
    float           invSignedArea;
    EdgeEquation    e0;             // P2 - P1
    EdgeEquation    e1;             // P0 - P2
    EdgeEquation    e2;             // P1 - P0
    glm::ivec2      min;            // AABB min
    glm::ivec2      max;            // AABB max

    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/
    
    /****************************************************************************//*!
    @brief    Constructor for AABB.

    @param[in]  p0
        A point on the edge of the triangle.
    @param[in]  p1
        A point on the edge of the triangle.
    @param[in]  p2
        A point on the edge of the triangle.
    *//*****************************************************************************/
    TriangleRasteriser(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2);

    TriangleRasteriser() = delete;

    /****************************************************************************//*!
    @brief    Checks whether a point is within the triangle.

    @param[in]  p0
        A point on the edge of the triangle.
    @param[in]  p1
        A point on the edge of the triangle.
    @param[in]  p2
        A point on the edge of the triangle.
    *//*****************************************************************************/
    bool PointInTriangle(const glm::vec2& point);
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

} // namespace GLUtility
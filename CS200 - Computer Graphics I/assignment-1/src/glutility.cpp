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

// Controller
// --------------------------------------------------------------------------------------
Controller::Controller(const int& _numObjects)
: object            (0)
, renderMode        (0)
, rotate            (false)
, numObjects        (_numObjects)
, numRenderModes    (1)
{}

// Color
// --------------------------------------------------------------------------------------
Color::Color(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
: r (_r)
, g (_g)
, b (_b)
, a (_a)
{}

Color::Color(const glm::vec3& colour, GLubyte _a)
: r (static_cast<GLubyte>(colour.r * 255))
, g (static_cast<GLubyte>(colour.g * 255))
, b (static_cast<GLubyte>(colour.b * 255))
, a (_a)
{}

Color::Color(const glm::dvec3& colour, GLubyte _a)
: Color(static_cast<glm::vec3>(colour), _a)
{};

// EdgeEquation
// --------------------------------------------------------------------------------------

EdgeEquation::EdgeEquation(const glm::dvec2& startPoint, const glm::dvec2& endPoint)
: normal    (startPoint.y - endPoint.y, endPoint.x - startPoint.x)
, c         ((startPoint.x * endPoint.y - endPoint.x * startPoint.y))
, topLeft   (normal.x != 0.0 ? normal.x > 0 : normal.y < 0)
{}

// Triangle Rasteriser
// --------------------------------------------------------------------------------------
TriangleRasteriser::TriangleRasteriser(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2)
: signedArea        ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y))
, invSignedArea     (signedArea != 0.f ? 1.f / signedArea : 0.f)
, e0                (p1, p2)
, e1                (p2, p0)
, e2                (p0, p1)
{
    min.x = static_cast<int>(std::min({ p0.x, p1.x, p2.x }));
    min.y = static_cast<int>(std::min({ p0.y, p1.y, p2.y }));
    max.x = static_cast<int>(ceil(std::max({ p0.x, p1.x, p2.x })));
    max.y = static_cast<int>(ceil(std::max({ p0.y, p1.y, p2.y })));
}

/*-------------------------------------------------------------------------------------*/
/* Member Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

// Controller
// --------------------------------------------------------------------------------------
void Controller::Update(GLboolean& objectKey, GLboolean& renderModeKey, GLboolean& toggleRotateKey)
{
    // Change task
    if (objectKey == GL_TRUE)
    {
        object == numObjects ? object = 0 : ++object;
        objectKey = GL_FALSE;

        #ifdef _DEBUG
        std::cout << "Object: " << object << std::endl;
        #endif
    }

    // Change modulate mode
    if (renderModeKey == GL_TRUE)
    {
        renderMode == numRenderModes ? renderMode = 0 : ++renderMode;
        renderModeKey = GL_FALSE;

        #ifdef _DEBUG
        std::cout << "Render Mode: " << renderMode << std::endl;
        #endif
    }

    // Change alpha blending mode
    if (toggleRotateKey == GL_TRUE)
    {
        rotate = !rotate;
        toggleRotateKey = GL_FALSE;

        #ifdef _DEBUG
        std::cout << "Rotate: " << (rotate ? "TRUE" : "FALSE") << std::endl;
        #endif
    }
}

// EdgeEquation
// --------------------------------------------------------------------------------------

double EdgeEquation::Evaluate(const glm::dvec2& point) const
{
    return (normal.x * point.x) + (normal.y * point.y) + c;
}

bool EdgeEquation::PointInEdgeTopLeft(const glm::dvec2& point) const
{
    double eval = Evaluate(point);
    return (eval > 0.0 || (eval == 0.0 && topLeft == true));
}

// TriangleRasteriser
// --------------------------------------------------------------------------------------
bool TriangleRasteriser::PointInTriangle(const glm::vec2& point)
{
    return e0.PointInEdgeTopLeft(point)
        && e1.PointInEdgeTopLeft(point)
        && e2.PointInEdgeTopLeft(point);
}
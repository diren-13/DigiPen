/************************************************************************************//*!
\file           rasteriser.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 18, 2021
\brief          This file contains the declaration of classes associated with rasterising
                primitive shapes onto a display. 

                Declared Classes:
                EdgeEquation
                TriangleRasteriser

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include <array>

#include <GL/glew.h>    // for access to OpenGL API declarations

#include <light.h>      // PointLight
#include <texture.h>        
#include <utility.h>    // Color

/************************************************************************************//*!
@brief    Encapsulates the implicit equation of an edge.
*//*************************************************************************************/
struct EdgeEquation
{
  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    glm::dvec2  p0;         // start point
    glm::dvec2  p1;         // end point
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

    /****************************************************************************//*!
    @brief    Function to rasterise a line using Bresenham's Algorithm.

    @param[in]  colour
        The colour of the line to be rendered.
    *//*****************************************************************************/
    void RenderLine(Colour colour) const;
};

/************************************************************************************//*!
@brief    Encapsulates the data of a triangle
*//*************************************************************************************/
struct Triangle
{
  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    static constexpr int TRI_PTS = 3;

    glm::dvec3* modelPos;
    glm::dvec3* devicePos;
    glm::dvec3* normals;
    glm::dvec2* texPos;

    double      signedAreaTimes2;
    double      inv2SignedArea;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/
    Triangle() = delete;

    /****************************************************************************//*!
    @brief      Constructor for Triangle.

    @param[in]  model
        The points of a triangle in model space. Used for lighting.
    @param[in]  device
        The points of a triangle in device space. Used for rasterisation.
    @param[in]  normal
        A normals of the triangle in NDC range. Used for lighting.
    @param[in]  texMap
        The texture mapping coordinates of the triangle.
    *//*****************************************************************************/
    Triangle(glm::dvec3* model, glm::dvec3* device, glm::dvec3* normal, glm::dvec2* texMap);

    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    inline glm::dvec3 GetCentroid()         const { return modelPos[0] + modelPos[1] + modelPos[2] / 3.0; }
    inline glm::dvec3 GetCentroidNormal()   const { return normals[0] + normals[1] + normals[2] / 3.0; }
};

/************************************************************************************//*!
@brief    Encapsulates a rasteriser for triangles
*//*************************************************************************************/
struct TriangleRasteriser
{
  private:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    Triangle        tri;
    EdgeEquation    e0;     // P2 - P1
    EdgeEquation    e1;     // P0 - P2
    EdgeEquation    e2;     // P1 - P0
    glm::ivec2      min;    // AABB min
    glm::ivec2      max;    // AABB max
    Texture*        tex;    // Pointer to texture                      

  private:
    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    /****************************************************************************//*!
    @brief      Function to compute the colour of a textured surface based on the 
                colour of the outgoing light calculated at the pixel.

    @param[in]  texelCoord
        The texel coordinate to get the texture color.
    @param[in]  lightColour
        The colour of the outgoing light calculated at the pixel.

    @returns  The texture colour modulated with the outgoing light colour.
    *//*****************************************************************************/
    glm::dvec3 ComputeTexturedColour(const glm::ivec2& texelCoord, const glm::dvec3& lightColour) const;

  public:
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief    Render Modes for the triangle
    *//*********************************************************************************/
    typedef enum class RENDER_MODE
    {
        SHADOW_MAP,
        FACETED,
        SMOOTH
    } RM;

    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/
    TriangleRasteriser() = delete;

    /****************************************************************************//*!
    @brief      Constructor for Triangle Rasteriser.

    @param[in]  model
        The points of a triangle in model space. Used for lighting.
    @param[in]  device
        The points of a triangle in device space. Used for rasterisation.
    @param[in]  normal
        A normals of the triangle in NDC range. Used for lighting.
    @param[in]  texMap
        The texture mapping coordinates of the triangle.
    @param[in]  texture
        The texture for the triangle.
    *//*****************************************************************************/
    TriangleRasteriser(glm::dvec3* model, glm::dvec3* device, glm::dvec3* normal, glm::dvec2* texMap, Texture* texture);

    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/
    
    /****************************************************************************//*!
    @brief      Function to rasterise a wirefrane triangle with the wireframe in the 
                specified colour.

                Checks if the triangle is front-faced (points passed in CCW) to 
                render. Culls back-faced triangles (CW).

    @param[in]  colour
        The colour of the wireframe.

    @returns    True if triangle is front faced. False if back faced.
    *//*****************************************************************************/
    bool RenderWireframe(Colour colour) const;

    /****************************************************************************//*!
    @brief      Function to rasterise a triangle based on the specified renderMode,
                a light source affected the triangle and whether texture mapping is
                applied.

                Checks if the triangle is front-faced (points passed in CCW) to 
                render. Culls back-faced triangles (CW).

    @param[in]  renderMode
        The rendering mode of the triangle. Use RM::SHADOWMAP, RM::FACTED or 
        RM::SMOOTH.
    @param[in]  lightSource
        The incoming light to the triangle.
    @param[in]  textured
        If texture mapping will be applied to the triangle.

    @returns    True if triangle is front faced. False if back faced.
    *//*****************************************************************************/
    bool RenderShadedTriangle(RENDER_MODE renderMode, PointLight& lightSource, bool textured = false) const;

    
};
/************************************************************************************//*!
\file           rasteriser.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 18, 2021
\brief          This file contains the implentation of classes declared in rasteriser.h.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <rasteriser.h>

#include <initializer_list>

#include <glpbo.h>

/*-------------------------------------------------------------------------------------*/
/* Constructors                                                                        */
/*-------------------------------------------------------------------------------------*/

// EdgeEquation
// --------------------------------------------------------------------------------------

EdgeEquation::EdgeEquation(const glm::dvec2& startPoint, const glm::dvec2& endPoint)
: p0        (startPoint)
, p1        (endPoint)
, normal    (p0.y - p1.y, p1.x - p0.x)
, c         ((p0.x * p1.y - p1.x * p0.y))
, topLeft   (normal.x != 0.0 ? normal.x > 0 : normal.y < 0)
{}

// Triangle
// --------------------------------------------------------------------------------------
Triangle::Triangle(glm::dvec3* model, glm::dvec3* device, glm::dvec3* normal, glm::dvec2* texMap)
: modelPos  (model)
, devicePos (device)
, normals   (normal)
, texPos    (texMap)
{
    signedAreaTimes2 = ((devicePos[1].x - devicePos[0].x) * (devicePos[2].y - devicePos[0].y) - (devicePos[2].x - devicePos[0].x) * (devicePos[1].y - devicePos[0].y));
    inv2SignedArea = (signedAreaTimes2 != 0.0 ? 1.0 / signedAreaTimes2 : 0.0);
}

// Triangle Rasteriser
// --------------------------------------------------------------------------------------
TriangleRasteriser::TriangleRasteriser(glm::dvec3* model, glm::dvec3* device, glm::dvec3* normal, glm::dvec2* texMap, Texture* texture)
: tri   (model, device, normal, texMap)
, e0    (tri.devicePos[1], tri.devicePos[2])
, e1    (tri.devicePos[2], tri.devicePos[0])
, e2    (tri.devicePos[0], tri.devicePos[1])
, tex   (texture)
{
    std::initializer_list<double> xCoords = { tri.devicePos[0].x, tri.devicePos[1].x, tri.devicePos[2].x };
    std::initializer_list<double> yCoords = { tri.devicePos[0].y, tri.devicePos[1].y, tri.devicePos[2].y };

    min.x = static_cast<int>(std::min(xCoords));
    min.y = static_cast<int>(std::min(yCoords));
    max.x = static_cast<int>(ceil(std::max(xCoords)));
    max.y = static_cast<int>(ceil(std::max(yCoords)));
}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions															   */
/*-------------------------------------------------------------------------------------*/

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

void EdgeEquation::RenderLine(Colour colour) const
{
    int dx = static_cast<int>(p1.x) - static_cast<int>(p0.x);
    int dy = static_cast<int>(p1.y) - static_cast<int>(p0.y);

    const int   X_STEP = (dx < 0) ? -1 : 1;
    const int   Y_STEP = (dy < 0) ? -1 : 1;

    // Abs dx and dy
    if (dx < 0) { dx = -dx; }
    if (dy < 0) { dy = -dy; }

    // Set first pixel
    int currentPixelX = static_cast<int>(p0.x);
    int currentPixelY = static_cast<int>(p0.y);
    GLPbo::SetPixel(currentPixelX, currentPixelY, colour);

    if (dx > dy)     // Always step in X
    {
        int decisionParam = (2 * dy) - dx;

        const int D_SINGLE_STEP = 2 * dy;
        const int D_DOUBLE_STEP = (2 * dy) - (2 * dx);

        while (dx)
        {
            --dx;

            decisionParam += (decisionParam > 0) ? D_DOUBLE_STEP : D_SINGLE_STEP;
            currentPixelY += (decisionParam > 0) ? Y_STEP : 0;
            currentPixelX += X_STEP;

            GLPbo::SetPixel(currentPixelX, currentPixelY, colour);
        }
    }
    else                // Always step in Y
    {
        int decisionParam = (2 * dx) - dy;

        const int D_SINGLE_STEP = 2 * dx;
        const int D_DOUBLE_STEP = (2 * dx) - (2 * dy);

        while (dy)
        {
            --dy;

            decisionParam += (decisionParam > 0) ? D_DOUBLE_STEP : D_SINGLE_STEP;
            currentPixelX += (decisionParam > 0) ? X_STEP : 0;
            currentPixelY += Y_STEP;

            GLPbo::SetPixel(currentPixelX, currentPixelY, colour);
        }
    }
}

// TriangleRasteriser
// --------------------------------------------------------------------------------------

bool TriangleRasteriser::RenderWireframe(Colour colour) const
{
    if (tri.signedAreaTimes2 <= 0.0) { return false; }

    e0.RenderLine(colour);     // Render e0
    e1.RenderLine(colour);     // Render e1
    e2.RenderLine(colour);     // Render e2

    return true;
}

bool TriangleRasteriser::RenderShadedTriangle(RM renderMode, PointLight& lightSource, bool textured) const
{
    if (tri.signedAreaTimes2 <= 0.0) { return false; }

    // Only used for faceted and smooth shaded
    glm::dvec3 vertexColours[Triangle::TRI_PTS]{};
    switch (renderMode)
    {
        case RM::FACETED:
        {
            vertexColours[0] = lightSource.ComputeOutgoingLight(tri.GetCentroid(), tri.GetCentroidNormal());
            break;
        }
        case RM::SMOOTH:
        {
            for (int i = 0; i < Triangle::TRI_PTS; ++i)
            {
                vertexColours[i] = lightSource.ComputeOutgoingLight(tri.modelPos[i], tri.normals[i]);
            }
            break;
        }
    }
    
    // Rasterise Triangle
    glm::dvec2 minCenter(min.x + 0.5, min.y + 0.5);

    double Eval0 = e0.Evaluate(minCenter);
    double Eval1 = e1.Evaluate(minCenter);
    double Eval2 = e2.Evaluate(minCenter);

    for (int y = min.y; y < max.y; ++y)
    {
        double hEval0 = Eval0, hEval1 = Eval1, hEval2 = Eval2;

        for (int x = min.x; x < max.x; ++x)
        {
            if (hEval0 >= 0.0 && hEval1 >= 0.0 && hEval2 >= 0.0)
            {
                // Compute barycentric constants
                double barycentricA = hEval0 * tri.inv2SignedArea;
                double barycentricB = hEval1 * tri.inv2SignedArea;
                double barycentricC = hEval2 * tri.inv2SignedArea;

                if (Utility::Scissor(x, y, GLPbo::GetWidth(), GLPbo::GetHeight()) == true)
                {
                    const double PIXEL_DEPTH = GLPbo::GetDepth(x, y);
                    const double DEPTH_VALUE = (tri.devicePos[0].z * barycentricA) + (tri.devicePos[1].z * barycentricB) + (tri.devicePos[2].z * barycentricC);

                    if (DEPTH_VALUE < PIXEL_DEPTH)
                    {
                        Colour pixelColour;

                        switch (renderMode)
                        {
                            case RM::SHADOW_MAP:
                            {
                                if (textured)
                                {
                                    glm::dvec2 textureCoord = (tri.texPos[0] * barycentricA) + (tri.texPos[1] * barycentricB) + (tri.texPos[2] * barycentricC);
                                    glm::ivec2 texelCoord(textureCoord.s * tex->GetWidth(), textureCoord.t * tex->GetHeight());

                                    if (Utility::Scissor(texelCoord.s, texelCoord.t, tex->GetWidth(), tex->GetHeight()) == true)
                                    {
                                        pixelColour = tex->GetTextureColour(texelCoord.s, texelCoord.t);
                                    }
                                }
                                else
                                {
                                    GLubyte colourValue = static_cast<GLubyte>(DEPTH_VALUE * 255);
                                    pixelColour = Colour(colourValue, colourValue, colourValue);
                                }
                                break;
                            }
                            case RM::FACETED:
                            {
                                if (textured)
                                {
                                    glm::dvec2 textureCoord = (tri.texPos[0] * barycentricA) + (tri.texPos[1] * barycentricB) + (tri.texPos[2] * barycentricC);
                                    glm::ivec2 texelCoord(textureCoord.s * tex->GetWidth(), textureCoord.t * tex->GetHeight());

                                    if (Utility::Scissor(texelCoord.s, texelCoord.t, tex->GetWidth(), tex->GetHeight()) == true)
                                    {
                                        pixelColour = ComputeTexturedColour(texelCoord, vertexColours[0]);
                                    }   
                                }
                                else
                                {
                                    pixelColour = vertexColours[0];
                                }
                                break;
                            }
                            case RM::SMOOTH:
                            {
                                glm::dvec3 lightColour = (vertexColours[0] * barycentricA) + (vertexColours[1] * barycentricB) + (vertexColours[2] * barycentricC);

                                if (textured)
                                {
                                    glm::dvec2 textureCoord = (tri.texPos[0] * barycentricA) + (tri.texPos[1] * barycentricB) + (tri.texPos[2] * barycentricC);
                                    glm::ivec2 texelCoord(textureCoord.s * tex->GetWidth(), textureCoord.t * tex->GetHeight());

                                    if (Utility::Scissor(texelCoord.s, texelCoord.t, tex->GetWidth(), tex->GetHeight()) == true)
                                    {
                                        pixelColour = ComputeTexturedColour(texelCoord, lightColour);
                                    }
                                }
                                else
                                {
                                    pixelColour = lightColour;
                                }
                                break;
                            }
                        }

                        GLPbo::SetDepth(x, y, DEPTH_VALUE);
                        GLPbo::SetPixel(x, y, pixelColour);
                    }
                }
            }

            // Update hEval by adding a0, a1, a2
            hEval0 += e0.normal.x;
            hEval1 += e1.normal.x;
            hEval2 += e2.normal.x;
        }

        // Update Eval by adding b0, b1, b2
        Eval0 += e0.normal.y;
        Eval1 += e1.normal.y;
        Eval2 += e2.normal.y;
    }

    return true;
}

glm::dvec3 TriangleRasteriser::ComputeTexturedColour(const glm::ivec2& texelCoord, const glm::dvec3& lightColour) const
{
    glm::dvec3 textureColour = tex->GetTextureColourNormalised(texelCoord.s, texelCoord.t);
    return lightColour * textureColour;
}
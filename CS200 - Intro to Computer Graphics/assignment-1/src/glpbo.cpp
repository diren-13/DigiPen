/************************************************************************************//*!
\file           glpbo.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           July 6, 2021
\brief          Contains implementation of functions declared in glpbo.h

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <glpbo.h>

#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include <iomanip>                  // setprecision

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>	// glm::pi
#include <dpml.h>

#include <glhelper.h>


/*-------------------------------------------------------------------------------------*/
/* Static Member Variables                                                             */
/*-------------------------------------------------------------------------------------*/
GLsizei		GLPbo::width;
GLsizei		GLPbo::height;
GLsizei		GLPbo::pixelCount;
GLsizei		GLPbo::byteCount;

Controller  GLPbo::controller;

Color*		GLPbo::pPBO = nullptr;
Color		GLPbo::clearColour;

GLuint		GLPbo::PBO;
GLuint		GLPbo::bufferTexObj;
GLSLShader	GLPbo::bufferShaderProgram;

int         GLPbo::culledCount;

std::vector<GLModel> GLPbo::models;

/*-------------------------------------------------------------------------------------*/
/* Private Member Functions															   */
/*-------------------------------------------------------------------------------------*/
void GLPbo::SetPixel(GLint x, GLint y, Color colour)
{
    // Scissor: check if coordinates are within window range
    if (x < 0 || x >= width || y < 0 || y >= height) { return; }
    
    *(pPBO + (y * width + x)) = colour;
}

void GLPbo::SetupPBO()
{
    // Create pbo
    glCreateBuffers(1, &PBO);
    glNamedBufferStorage(PBO, byteCount, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

    // Set buffer colour
    pPBO = static_cast<Color*>(glMapNamedBuffer(PBO, GL_WRITE_ONLY));
    ClearColourBuffer();
    glUnmapNamedBuffer(PBO);

    // Create Quad
    GLModel bgQuad;
    bgQuad.InitWindowSizedQuad();
    models.emplace_back(bgQuad);

    // Create texture obj
    glCreateTextures(GL_TEXTURE_2D, 1, &bufferTexObj);
    glTextureStorage2D(bufferTexObj, 1, GL_RGBA8, width, height);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glTextureSubImage2D(bufferTexObj, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    SetupPBOShader();
}

void GLPbo::SetupPBOShader()
{
    const std::string vertexShaderSource =
    "#version 450 core\n"
    "layout(location = 0)	in  vec2 aVertexPosition;\n"
    "layout(location = 3)	in	vec2 aTextureCoordinate;\n"
    "layout(location = 0)	out vec2 vTextureCoordinate;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = vec4(aVertexPosition, 0.0, 1.0);\n"
    "	vTextureCoordinate = aTextureCoordinate;\n"
    "}\n";

    const std::string fragmentShaderSource =
    "#version 450 core\n"
    "uniform sampler2D uTex2D;\n"
    "layout(location = 0)	in 	vec2 vTextureCoordinate;\n"
    "layout(location = 0)	out vec4 fFragColour;\n"
    "void main()\n"
    "{\n"
    "	fFragColour = texture(uTex2D, vTextureCoordinate);\n"
    "}\n";

    // Compile-Link-Validate
    bufferShaderProgram.CompileShaderFromString(GL_VERTEX_SHADER, vertexShaderSource);
    bufferShaderProgram.CompileShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderSource);
    bufferShaderProgram.Link();
    bufferShaderProgram.Validate();

    // Handle failure to link
    if (bufferShaderProgram.IsLinked() == GL_FALSE)
    {
        std::cout << "Unable to compile/link/validate shader programs!" << std::endl;
        std::cout << bufferShaderProgram.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    #ifdef _DEBUG
    std::cout << "Active Shader Attributes\n" << "----------------------------------------------------------------------\n";
    bufferShaderProgram.PrintActiveAttribs();
    std::cout << "\nActive Shader Uniforms\n" << "----------------------------------------------------------------------\n";
    bufferShaderProgram.PrintActiveUniforms();
    std::cout << std::endl;
    #endif

    // Delete shaders
    std::array<GLuint, 2> attachedShaders;
    GLsizei numAttachedShaders;
    glGetAttachedShaders(bufferShaderProgram.GetHandle(), 2, &numAttachedShaders, attachedShaders.data());

    for (auto& shader : attachedShaders) { glDeleteShader(shader); }
}

void GLPbo::SetupScene(const std::string& filename)
{
    std::ifstream ifs(filename, std::ios::in);

    // If file fails to open
    if (!ifs)
    {
        std::cout << "ERROR: Unable to open scene file: " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Set file cursor to beginnning of the file
    ifs.seekg(0, std::ios::beg);
    
    while (!ifs.eof())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty()) { continue; }

        GLModel tmp;
        if (tmp.InitModel(line) == true)
        {
            models.emplace_back(tmp);
        }
    }

    ifs.close();

    #ifdef _DEBUG
    std::cout << "\nScene setup COMPLETE!\n"
              << "----------------------------------------------------------------------\n" 
              << std::endl;
    #endif
}

void GLPbo::ClearColourBuffer()
{
    std::fill_n(pPBO, pixelCount, clearColour);
}

void GLPbo::ViewportTransform(GLModel& model)
{
    const float HALF_WIDTH  = width * 0.5f;
    const float HALF_HEIGHT = height * 0.5f;

    const float COS_ROTATION = cos(model.rotation);
    const float SIN_ROTATION = sin(model.rotation);

    glm::mat4 transform =
    {
        HALF_WIDTH * COS_ROTATION,  HALF_HEIGHT * SIN_ROTATION, 0.f, 0.f,
        HALF_WIDTH * -SIN_ROTATION, HALF_HEIGHT * COS_ROTATION, 0.f, 0.f,
        0.f,                        0.f,                        1.f, 0.f,
        HALF_WIDTH,                 HALF_HEIGHT,                0.f, 1.f,
    };


    for (auto modelPosIter = model.posVtx.begin(), devicePosIter = model.devicePos.begin(); 
         modelPosIter != model.posVtx.end(); ++modelPosIter, ++devicePosIter)
    {
        *devicePosIter = transform * glm::vec4(modelPosIter->x, modelPosIter->y, modelPosIter->z, 1.f);
        devicePosIter->z = 0.f;

        if (devicePosIter == model.devicePos.end()) { break; }
    }
}

void GLPbo::RenderLine(const glm::vec2& p0, const glm::vec2& p1, const Color& drawColour)
{
    int dx = static_cast<int>(p1.x) - static_cast<int>(p0.x);
    int dy = static_cast<int>(p1.y) - static_cast<int>(p0.y);

    const int   X_STEP  = (dx < 0) ? -1 : 1;
    const int   Y_STEP  = (dy < 0) ? -1 : 1;

    // Abs dx and dy
    if (dx < 0) { dx = -dx; }
    if (dy < 0) { dy = -dy; }

    // Set first pixel
    int currentPixelX = static_cast<int>(p0.x);
    int currentPixelY = static_cast<int>(p0.y);
    SetPixel(currentPixelX, currentPixelY, drawColour);

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

            SetPixel(currentPixelX, currentPixelY, drawColour);
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

            SetPixel(currentPixelX, currentPixelY, drawColour);
        }
    }
}

bool GLPbo::RenderWireframeTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const Color& colour)
{
    const float signedAreaOfTriangle = ((p1.x - p0.x) * (p2.y - p0.y)) - ((p2.x - p0.x) * (p1.y - p0.y));
    if (signedAreaOfTriangle <= 0.f) { return false; }

    RenderLine(p1, p2, colour);     // Render e0
    RenderLine(p2, p0, colour);     // Render e1
    RenderLine(p0, p1, colour);     // Render e2

    return true;
}

bool GLPbo::RenderFacetedTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const Color& colour)
{
    TriangleRasteriser tri(p0, p1, p2);

    if (tri.signedArea <= 0) { return false; }

    // Rasterise Triangle
    glm::dvec2 minCenter(tri.min.x + 0.5, tri.min.y + 0.5);

    double Eval0 = tri.e0.Evaluate(minCenter);
    double Eval1 = tri.e1.Evaluate(minCenter);
    double Eval2 = tri.e2.Evaluate(minCenter);

    for (int y = tri.min.y; y < tri.max.y; ++y)
    {
        double hEval0 = Eval0, hEval1 = Eval1, hEval2 = Eval2;

        for (int x = tri.min.x; x < tri.max.x; ++x)
        {
            if (hEval0 >= 0.0 && hEval1 >= 0.0 && hEval2 >= 0.0) { SetPixel(x, y, colour); }

            // Update hEval by adding a0, a1, a2
            hEval0 += tri.e0.normal.x;
            hEval1 += tri.e1.normal.x;
            hEval2 += tri.e2.normal.x;
        }

        // Update Eval by adding b0, b1, b2
        Eval0 += tri.e0.normal.y;
        Eval1 += tri.e1.normal.y;
        Eval2 += tri.e2.normal.y;
    }

    return true;
}

bool GLPbo::RenderShadedTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
                                 const glm::dvec3& c0, const glm::dvec3& c1, const glm::dvec3& c2)
{
    TriangleRasteriser tri(p0, p1, p2);

    if (tri.signedArea <= 0) { return false; }

    // Rasterise Triangle
    glm::dvec2 minCenter(tri.min.x + 0.5, tri.min.y + 0.5);

    double Eval0 = tri.e0.Evaluate(minCenter);
    double Eval1 = tri.e1.Evaluate(minCenter);
    double Eval2 = tri.e2.Evaluate(minCenter);

    for (int y = tri.min.y; y < tri.max.y; ++y)
    {
        double hEval0 = Eval0, hEval1 = Eval1, hEval2 = Eval2;

        for (int x = tri.min.x; x < tri.max.x; ++x)
        {
            // Compute barycentric constants
            double barycentricA = hEval0 * tri.invSignedArea;
            double barycentricB = hEval1 * tri.invSignedArea;
            double barycentricC = hEval2 * tri.invSignedArea;

            // Compute interpolated colour
            glm::dvec3 drawColour = (c0 * barycentricA) + (c1 * barycentricB) + (c2 * barycentricC);

            if (hEval0 >= 0.0 && hEval1 >= 0.0 && hEval2 >= 0.0) { SetPixel(x, y, drawColour); }

            hEval0 += tri.e0.normal.x;
            hEval1 += tri.e1.normal.x;
            hEval2 += tri.e2.normal.x;
        }

        Eval0 += tri.e0.normal.y;
        Eval1 += tri.e1.normal.y;
        Eval2 += tri.e2.normal.y;
    }

    return true;
}

void GLPbo::RenderModel(GLModel& model, const int& renderMode)
{
    // Reset this every frame
    culledCount = 0;

    ViewportTransform(model);

    const size_t IDX_SIZE = model.idxVtx.size();

    for (size_t idx = 0, clrIdx = 0; idx < IDX_SIZE; idx += 3, ++clrIdx)
    {
        glm::vec3 points[3];

        for (int i = 0; i < 3; ++i)
        {
            GLushort elementIdx = model.idxVtx[idx + i];
            points[i] = model.devicePos[elementIdx];
        }

        switch (renderMode)
        {
            case 0:     // Wireframe
            {
                if (RenderWireframeTriangle(points[0], points[1], points[2]) == false) 
                { 
                    ++culledCount; 
                }
                break;
            }
            case 1:     // Wireframe Colour
            {
                if (RenderWireframeTriangle(points[0], points[1], points[2], model.colour[clrIdx]) == false)
                { 
                    ++culledCount; 
                }
                break;
            }
            case 2:     // Faceted
            {
                if (RenderFacetedTriangle(points[0], points[1], points[2], model.colour[clrIdx]) == false)
                { 
                    ++culledCount; 
                }
                break;
            }
            case 3:     // Shaded
            {
                glm::vec3 normals[3];

                for (int i = 0; i < 3; ++i)
                {
                    GLushort elementIdx = model.idxVtx[idx + i];
                    normals[i] = model.normalVtx[elementIdx];
                }

                if (RenderShadedTriangle(points[0], points[1], points[2], normals[0], normals[1], normals[2]) == false)
                {
                    ++culledCount;
                }
                break;
            }
        }
    }
}

/*-------------------------------------------------------------------------------------*/
/* Public Member Functions															   */
/*-------------------------------------------------------------------------------------*/
void GLPbo::emulate()
{
    controller.Update(GLHelper::keystateM, GLHelper::keystateW, GLHelper::keystateR);

    // Update rotation of model
    GLModel& currentModel = models[controller.object + 1];
    if (controller.rotate == true) { currentModel.rotation += 0.01f; }
    
    pPBO = static_cast<Color*>(glMapNamedBuffer(PBO, GL_WRITE_ONLY));
    ClearColourBuffer();

    RenderModel(currentModel, controller.renderMode);
    
    glUnmapNamedBuffer(PBO);

    glTextureSubImage2D(bufferTexObj, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    SetTitleBar();
}

void GLPbo::SetTitleBar()
{
    std::stringstream sstr;

    // Title
    sstr << "CSD2100";

    // Object
    sstr << " | Object: ";
    switch (controller.object)
    {
        case 0:     // Cube
        {
            sstr << "Cube";
            break;
        }
        case 1:     // Suzanne
        {
            sstr << "Suzanne";
            break;
        }
        case 2:     // Ogre
        {
            sstr << "Ogre";
            break;
        }
        case 3:     // Head
        {
            sstr << "Head";
            break;
        }
        case 4:     // Teapot
        {
            sstr << "Teapot";
            break;
        }
    }

    // Render Mode
    sstr << " | Mode: ";
    switch (controller.renderMode)
    {
        case 0:     // Wireframe
        {
            sstr << "Wireframe";
            break;
        }
        case 1:     // Wireframe Colour
        {
            sstr << "Wireframe Colour";
            break;
        }
        case 2:     // Faceted
        {
            sstr << "Faceted";
            break;
        }
        case 3:     // Shaded
        {
            sstr << "Shaded";
            break;
        }
    }

    // Num Vertex
    sstr << " | Vertex: ";
    sstr << models[controller.object + 1].posVtx.size();

    // Num Triangles
    sstr << " | Triangle: ";
    sstr << models[controller.object + 1].colour.size();

    // Num Culled
    sstr << " | Culled: ";
    sstr << culledCount;

    // -- FPS
    sstr << std::fixed << std::setprecision(2) << " | FPS";

    GLHelper::title = sstr.str();
}

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions																   */
/*-------------------------------------------------------------------------------------*/

void GLPbo::init(GLsizei w, GLsizei h)
{
    // Assign values
    width		= w;
    height		= h;
    pixelCount	= width * height;
    byteCount	= sizeof(Color) * pixelCount;

    // Init tracking variables
    culledCount     = 0;

    SetClearColour(255, 255, 255);
    SetupPBO();
    SetupScene("../scenes/ass-1.scn");

    controller.SetNumObjects(5);
    controller.SetNumRenderModes(4);
    
    #ifdef _DEBUG
    // Print current GPU info to CLI
    GLHelper::print_specs();
    #endif
}

void GLPbo::draw_fullwindow_quad()
{
    glBindTextureUnit(0, bufferTexObj);

    bufferShaderProgram.Use();
    bufferShaderProgram.SetUniform("uTex2D", 0);

    glBindVertexArray(models[0].VAO);
    glDrawElements(models[0].primitiveType, models[0].drawCount, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);

    bufferShaderProgram.UnUse();
}

void GLPbo::cleanup()
{
    // Delete buffers & VAO
    for (auto& model : models) { model.Release(); }
    glDeleteBuffers(1, &PBO);

    // Delete texture objects
    glDeleteTextures(1, &bufferTexObj);

    // Delete shader programs
    bufferShaderProgram.DeleteShaderProgram();
}

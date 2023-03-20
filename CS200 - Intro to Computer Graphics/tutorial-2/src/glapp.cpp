/************************************************************************************//*!
\file           glapp.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 13, 2021
\brief          This file implements functionality useful and necessary to build OpenGL
                applications including use of external APIs such as GLFW to create a
                window and start up an OpenGL context and to extract function pointers
                to OpenGL implementations.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

/*                                                                            includes
-------------------------------------------------------------------------------------- */
#include <iostream>
#include <iomanip>
#include <array>
#include <utility>	// std::pair

#include <chrono>
#include <random>   // std::default_random_generator

#include <glapp.h>
#include <glm/gtc/constants.hpp>    // glm::pi

/*                                                                             defines
-------------------------------------------------------------------------------------- */


/*                                                             objects with file scope
-------------------------------------------------------------------------------------- */

// Definition for static member function of GLApp
std::vector<GLApp::GLViewport>  GLApp::vps;
std::vector<GLApp::GLModel>     GLApp::models;

/*                                                               function declarations
-------------------------------------------------------------------------------------- */
float   randFloat();

/*                                                                function definitions
-------------------------------------------------------------------------------------- */

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions                                                                 */
/*-------------------------------------------------------------------------------------*/

void GLApp::init() 
{
    // Fill back buffer with colour
    glClearColor(1.f, 1.f, 1.f, 1.f);

    // Viewports
    // ----------------------------------------------------------------------------------
    // Set the 4 viewports
    GLint halfWidth     = static_cast<GLint>(GLHelper::width * 0.5f);
    GLint halfHeight    = static_cast<GLint>(GLHelper::height * 0.5f);

    // In order: Top-Left, Top-Right, Bottom-Left, Bottom-Right
    vps.emplace_back(0, halfHeight, halfWidth, halfHeight);
    vps.emplace_back(halfWidth, halfHeight, halfWidth, halfHeight);
    vps.emplace_back(0, 0, halfWidth, halfHeight);
    vps.emplace_back(halfWidth, 0, halfWidth, halfHeight);
    // ----------------------------------------------------------------------------------

    // Models
    // ----------------------------------------------------------------------------------
    std::string vShaderPath = "../shaders/tutorial-2.vert";
    std::string fShaderPath = "../shaders/tutorial-2.frag";

    // Points
    const int P_SLICES = 20;
    const int P_STACKS = 20;
    GLModel points = pointsModel(P_SLICES, P_STACKS, vShaderPath, fShaderPath);
    models.emplace_back(points);
    // Lines
    const int L_SLICES = 40;
    const int L_STACKS = 40;
    GLModel lines = linesModel(L_SLICES, L_STACKS, vShaderPath, fShaderPath);
    models.emplace_back(lines);
    // Disc
    const int F_SLICES = 50;
    GLModel trifan = trifanModel(F_SLICES, vShaderPath, fShaderPath);
    models.emplace_back(trifan);
    // Strips
    const int S_SLICES = 10;
    const int S_STACKS = 15;
    GLModel tristrips = tristripModel(S_SLICES, S_STACKS, vShaderPath, fShaderPath);
    models.emplace_back(tristrips);
    // ----------------------------------------------------------------------------------

    // Print current GPU info to CLI
    GLHelper::printGPUSpecs();
}

void GLApp::update(double delta_time) 
{
    // Empty function
}

void GLApp::draw()
{
    // Set title bar
    // ----------------------------------------------------------------------------------
    std::string title = GLHelper::title;
    
    for (auto& model : models)
    {
        title += " | ";
        
        switch (model.primitiveType)
        {
            case GL_POINTS:
            {
                title += "GL_POINTS ";
                break;
            }
            case GL_LINES:
            {
                title += "GL_LINES ";
                break;
            }
            case GL_TRIANGLE_FAN:
            {
                title += "GL_TRIANGLE_FAN ";
                break;
            }
            case GL_TRIANGLE_STRIP:
            {
                title += "GL_TRIANGLE_STRIP ";
                break;
            }
        }
        title += "cnt: " + std::to_string(model.primitiveCount);
        title += " vtx: " + std::to_string(model.drawCount);
    }
    title += " | ";

    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(2) << "fps: " << GLHelper::fps;
    title += sstr.str();

    glfwSetWindowTitle(GLHelper::ptr_window, title.c_str());
    // ----------------------------------------------------------------------------------

    // Send back buffer information to GPU
    glClear(GL_COLOR_BUFFER_BIT);

    for (size_t i = 0; i < vps.size(); ++i)
    {
        glViewport(vps[i].x, vps[i].y, vps[i].width, vps[i].height);
        models[i].Draw();
    }
}
 
void GLApp::cleanup() 
{
    
}

/*-------------------------------------------------------------------------------------*/
/* Function Members																       */
/*-------------------------------------------------------------------------------------*/
GLApp::GLModel GLApp::pointsModel(int slices, int stacks, std::string vShader, std::string fShader)
{
    const float             BOUNDS          = 1.f - (-1.f);     // upper bound - lower bound
    const float             SLICE_INTERVAL  = BOUNDS / slices;
    const float             STACK_INTERVAL  = BOUNDS / stacks;

    std::vector<glm::vec2>  posVtx;
    std::vector<glm::vec3>  colourVtx;

    // Get all positions and colour vertex data
    for (int i = 0; i <= stacks; ++i)
    {
        for (int j = 0; j <= slices; ++j)
        {
            posVtx.emplace_back
            (
                (SLICE_INTERVAL * j) - 1.f,
                (STACK_INTERVAL * i) - 1.f
            );
            colourVtx.emplace_back(randFloat(), randFloat(), randFloat());
        }
    }

    // Send data to GPU
    // ----------------------------------------------------------------------------------
    GLuint vboHandle;   // Create memory on GPU (VBO) to hold vertex data
    GLuint vaoPoints;   // Create VAO to access VBO data

    const GLsizeiptr POS_SIZE    = sizeof(glm::vec2) * posVtx.size();
    const GLsizeiptr COLOUR_SIZE = sizeof(glm::vec3) * colourVtx.size();

    glCreateBuffers(1, &vboHandle);
    glCreateVertexArrays(1, &vaoPoints);

    glNamedBufferStorage(vboHandle, POS_SIZE + COLOUR_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(vboHandle, 0, POS_SIZE, posVtx.data());
    glNamedBufferSubData(vboHandle, POS_SIZE, COLOUR_SIZE, colourVtx.data());

    BindVBOPoints(vaoPoints, vboHandle);
    BindVBOColour(vaoPoints, vboHandle, POS_SIZE);

    glBindVertexArray(0);
    // ----------------------------------------------------------------------------------

    // Create model
    GLModel mdl;
    mdl.vaoid           = vaoPoints;
    mdl.primitiveType   = GL_POINTS;
    mdl.drawCount       = posVtx.size();    // Num vertices
    mdl.primitiveCount  = mdl.drawCount;    // Num primitives
    mdl.Setup_ShaderProgram(vShader, fShader);
    return mdl;
}

GLApp::GLModel GLApp::linesModel(int slices, int stacks, std::string vShader, std::string fShader)
{
    const float             BOUNDS          = 1.f - (-1.f);     // upper bound - lower bound
    const float             SLICE_SEGMENT = BOUNDS / slices;
    const float             STACK_SEGMENT = BOUNDS / stacks;

    std::vector<glm::vec2>  posVtx;

    // Vertical Lines
    for (int col = 0; col <= slices; ++col)
    {
        float x = (SLICE_SEGMENT * col) - 1.f;

        posVtx.emplace_back(x, -1.f);
        posVtx.emplace_back(x, 1.f);
    }
    // Horizontal Lines
    for (int row = 0; row <= stacks; ++row)
    {
        float y = (STACK_SEGMENT * row) - 1.f;

        posVtx.emplace_back(-1.f, y);
        posVtx.emplace_back(1.f, y);
    }

    // Send data to GPU
    // ----------------------------------------------------------------------------------
    GLuint vboHandle;
    GLuint vaoLines;

    const GLsizeiptr POS_SIZE = sizeof(glm::vec2) * posVtx.size();

    glCreateBuffers(1, &vboHandle);
    glCreateVertexArrays(1, &vaoLines);

    glNamedBufferStorage(vboHandle, POS_SIZE, posVtx.data(), GL_DYNAMIC_STORAGE_BIT);

    BindVBOPoints(vaoLines, vboHandle);

    glBindVertexArray(0);
    // ----------------------------------------------------------------------------------


    // Create model
    GLModel mdl;
    mdl.vaoid           = vaoLines;
    mdl.primitiveType   = GL_LINES;
    mdl.drawCount       = (slices + 1) * 2 + (stacks + 1) * 2;  // Num vertices
    mdl.primitiveCount  = mdl.drawCount / 2;                    // Num primitives
    mdl.Setup_ShaderProgram(vShader, fShader);
    return mdl;
}

GLApp::GLModel GLApp::trifanModel(int slices, std::string vShader, std::string fShader)
{
    const int               NUM_VTX         = slices + 2;
    const float             ANGLE_INTERVAL  = 2.f * glm::pi<float>() / slices;

    std::vector<glm::vec2>  posVtx;
    std::vector<glm::vec3>  colourVtx;

    // Vertex Data
    // ----------------------------------------------------------------------------------
    posVtx.emplace_back(0.f, 0.f);      // Put in v[0] at the front of the posVtx
    for (int i = 0; i < NUM_VTX; ++i)
    {
        float angle = ANGLE_INTERVAL * i; 
        posVtx.emplace_back(cos(angle), sin(angle));

        colourVtx.emplace_back(randFloat(), randFloat(), randFloat());
    }
    // Set v[last] == v[1]
    colourVtx.back() = colourVtx[1];
    // ----------------------------------------------------------------------------------

    // Send data to GPU
    // ----------------------------------------------------------------------------------
    GLuint vboHandle;
    GLuint vaoTrifan; 

    const GLsizeiptr POS_SIZE      = sizeof(glm::vec2) * posVtx.size();
    const GLsizeiptr COLOUR_SIZE   = sizeof(glm::vec3) * colourVtx.size();

    glCreateBuffers(1, &vboHandle);
    glCreateVertexArrays(1, &vaoTrifan);

    glNamedBufferStorage(vboHandle, POS_SIZE + COLOUR_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(vboHandle, 0, POS_SIZE, posVtx.data());
    glNamedBufferSubData(vboHandle, POS_SIZE, COLOUR_SIZE, colourVtx.data());

    BindVBOPoints(vaoTrifan, vboHandle);
    BindVBOColour(vaoTrifan, vboHandle, POS_SIZE);

    glBindVertexArray(0);
    // ----------------------------------------------------------------------------------

    GLModel mdl;
    mdl.vaoid           = vaoTrifan;
    mdl.primitiveType   = GL_TRIANGLE_FAN;
    mdl.drawCount       = NUM_VTX;       // Num vertices
    mdl.primitiveCount  = NUM_VTX - 2;   // Num primitives
    mdl.Setup_ShaderProgram(vShader, fShader);
    return mdl;
}

GLApp::GLModel GLApp::tristripModel(int slices, int stacks, std::string vShader, std::string fShader)
{
    const float             BOUNDS = 1.f - (-1.f);     // upper bound - lower bound
    const float             SLICE_INTERVAL = BOUNDS / slices;
    const float             STACK_INTERVAL = BOUNDS / stacks;

    std::vector<glm::vec2>  posVtx;
    std::vector<glm::vec3>  colourVtx;
    std::vector<GLushort>   idxVtx;

    for (int i = 0; i <= stacks; ++i)
    {
        for (int j = 0; j <= slices; ++j)
        {
            posVtx.emplace_back
            (
                (SLICE_INTERVAL * j) - 1.f,
                (STACK_INTERVAL * i) - 1.f
            );
            colourVtx.emplace_back(randFloat(), randFloat(), randFloat());
        }
    }

    // Set indices
    GLushort btmIdx         = 0;
    GLushort slicePlusOne   = static_cast<GLushort>(slices + 1);
    for (int i = 0; i < stacks; ++i)
    {
        // Append leading number if not first stack
        if (i) { idxVtx.emplace_back(slicePlusOne + btmIdx); }
       
        for (int j = 0; j <= slices; ++j)
        {
            idxVtx.emplace_back(slicePlusOne + btmIdx);
            idxVtx.emplace_back(btmIdx++);
        }

        // Append trailing number if not last stack
        if (i < stacks - 1) { idxVtx.emplace_back(idxVtx.back()); }
    }

    // Send data to GPU
    // ----------------------------------------------------------------------------------
    GLuint vboHandle;
    GLuint vaoTristrip;
    GLuint eboHandle;

    const GLsizeiptr POS_SIZE      = sizeof(glm::vec2) * posVtx.size();
    const GLsizeiptr COLOUR_SIZE   = sizeof(glm::vec3) * colourVtx.size();

    glCreateBuffers(1, &vboHandle);
    glCreateVertexArrays(1, &vaoTristrip);
    glCreateBuffers(1, &eboHandle);

    glNamedBufferStorage(vboHandle, POS_SIZE + COLOUR_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(vboHandle, 0, POS_SIZE, posVtx.data());
    glNamedBufferSubData(vboHandle, POS_SIZE, COLOUR_SIZE, colourVtx.data());

    glNamedBufferStorage(eboHandle, sizeof(GLushort) * idxVtx.size(), 
                         reinterpret_cast<GLvoid*>(idxVtx.data()), GL_DYNAMIC_STORAGE_BIT);

    BindVBOPoints(vaoTristrip, vboHandle);
    BindVBOColour(vaoTristrip, vboHandle, POS_SIZE);

    glVertexArrayElementBuffer(vaoTristrip, eboHandle);
    
    glBindVertexArray(0);
    // ----------------------------------------------------------------------------------

    GLModel mdl;
    mdl.vaoid           = vaoTristrip;
    mdl.primitiveType   = GL_TRIANGLE_STRIP;
    mdl.drawCount       = idxVtx.size();        // Num indices
    mdl.primitiveCount  = idxVtx.size() - 2;    // Num primitives
    mdl.Setup_ShaderProgram(vShader, fShader);
    return mdl;
}

/*-------------------------------------------------------------------------------------*/
/* Helper Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

void GLApp::BindVBOPoints(GLuint& vaobj, GLuint& buffer)
{
    constexpr const GLuint POS_ATTRIB     = 0;
    constexpr const GLuint POS_BINDING_PT = 0;

    // -- Enable, Prepare Bind, Format, Bind
    glEnableVertexArrayAttrib(vaobj, POS_ATTRIB);
    glVertexArrayVertexBuffer(vaobj, POS_BINDING_PT, buffer, 0, sizeof(glm::vec2));
    glVertexArrayAttribFormat(vaobj, POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaobj, POS_ATTRIB, POS_BINDING_PT);
}

void GLApp::BindVBOColour(GLuint& vaobj, GLuint& buffer, const GLsizeiptr& offset)
{
    constexpr const GLuint CLR_ATTRIB       = 1;
    constexpr const GLuint CLR_BINDING_PT   = 1;

    glEnableVertexArrayAttrib(vaobj, CLR_ATTRIB);
    glVertexArrayVertexBuffer(vaobj, CLR_BINDING_PT, buffer, offset, sizeof(glm::vec3));
    glVertexArrayAttribFormat(vaobj, CLR_ATTRIB, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaobj, CLR_ATTRIB, CLR_BINDING_PT);
}

/*-------------------------------------------------------------------------------------*/
/* GLModel Functions                                                                   */
/*-------------------------------------------------------------------------------------*/

void GLApp::GLModel::Setup_ShaderProgram(std::string vShader, std::string fShader)
{
    // Shader Program Flow
    // 1. Create Shader
    // 2. Get Vertex Shader Source Code from strings
    // 3. Compile Vertex Shader Source Code
    // 4. Create Shader Program
    // 5. Get Fragment Shader Source Code, attach to Shader Program
    // 6. Link Vertex & Fragment Shaders into Single Shader Program (Make sure to verify link)
    // 7. Install Shader Program on OpenGL pipeline using glUseProgram

    // NOTE: This has been heavily abstracted. Dive into the definitions of the functions
    // to see the implementation of the above steps.

    std::vector<std::pair<GLenum, std::string>> shaderFiles;

    shaderFiles.emplace_back(std::make_pair(GL_VERTEX_SHADER, vShader));
    shaderFiles.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, fShader));

    shaderProgram.CompileLinkValidate(shaderFiles);

    if (shaderProgram.IsLinked() == GL_FALSE)
    {
        std::cout << "Unable to compile/link/validate shader programs\n";
        std::cout << shaderProgram.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void GLApp::GLModel::Draw()
{
    // Use the shader
    shaderProgram.Use();

    // Bind VAO for rendering
    glBindVertexArray(vaoid);

    switch (primitiveType)
    {
        case GL_POINTS:
        {
            glPointSize(10.f);
            glDrawArrays(primitiveType, 0, drawCount);
            glPointSize(1.f);
            break;
        }
        case GL_LINES:
        {
            glLineWidth(3.f);
            glVertexAttrib3f(1, 0.f, 0.f, 1.f);         // Blue colour
            glDrawArrays(primitiveType, 0, drawCount);
            glLineWidth(1.f);
            break;
        }
        case GL_TRIANGLE_FAN:
        {
            glDrawArrays(primitiveType, 0, drawCount);
            break;
        }
        case GL_TRIANGLE_STRIP:
        {
            glDrawElements(primitiveType, drawCount, GL_UNSIGNED_SHORT, NULL);
            break;
        }
    }

    glBindVertexArray(0);
    shaderProgram.UnUse();
}

/*-------------------------------------------------------------------------------------*/
/* Helper Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
@brief     Gets a random float between a normalised range of 1.0 and 0.0
*//*************************************************************************************/
float randFloat()
{
    //return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());

    std::default_random_engine generator(seed);

    std::uniform_real_distribution<float> distribution(0.f, 1.f);

    return distribution(generator);
}

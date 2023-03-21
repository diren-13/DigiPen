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

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <utility>	// std::pair

#include <glapp.h>

/*                                                                    defines
----------------------------------------------------------------------------- */


/*                                                   objects with file scope
----------------------------------------------------------------------------- */

static Colour	greenColour;
static bool		colourLerpState;	// true if green to red, false if red to green
static GLfloat	colourLerpValue;	// the current green value. Red value is 1 - this value.

// Definition for static member function of GLApp
GLApp::GLModel GLApp::mdl;

/*                                                      function declarations
----------------------------------------------------------------------------- */
void printContextInfo();

void colourLerp(bool&, GLfloat&, GLfloat);

void buildAttribsArray(std::vector<GLint>&, const GLint&);
void buildBindingsArray(std::vector<GLint>&, const GLint&);

/*                                                       function definitions
----------------------------------------------------------------------------- */

/*-----------------------------------------------------------------------------*/
/* Lifecycle Functions                                                         */
/*-----------------------------------------------------------------------------*/

void GLApp::init() 
{
    // Set initial values for green colour
    greenColour = { 0.f, 1.f, 0.f, 1.f };
    
    // Set initial lerp vars
    colourLerpState = true;		// Start lerping immediately
    colourLerpValue = greenColour.G;

    // Fill back buffer with colour
    glClearColor(greenColour.R, greenColour.G, greenColour.B, greenColour.A);

    // Set viewport to entire window
    glViewport(0, 0, GLHelper::width, GLHelper::height);

    mdl.Setup_VAO();
    mdl.Setup_ShaderProgram();

    // Print current openGL context info to CLI
    printContextInfo();
}

void GLApp::update(double delta_time) 
{
    GLfloat lerpAmount = static_cast<GLfloat>(0.25f * delta_time);
    colourLerp(colourLerpState, colourLerpValue, lerpAmount);

    // Fill back buffer with colour
    glClearColor(1 - colourLerpValue, colourLerpValue, greenColour.B, greenColour.A);
}

void GLApp::draw() 
{
    // Send back buffer information to GPU
    glClear(GL_COLOR_BUFFER_BIT);

    mdl.Draw();
}

void GLApp::cleanup() 
{
    
}

/*-----------------------------------------------------------------------------*/
/* GLModel Functions                                                           */
/*-----------------------------------------------------------------------------*/
void GLApp::GLModel::Setup_VAO()
{
    // Define rectangle vertices and colour vertices in NDC
    // -- Move in CCW Direction
    std::array<glm::vec2, 4> posVtx
    {
        glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.5f),
        glm::vec2(-0.5f, 0.5f), glm::vec2(-0.5f, -0.5f)
    };

    std::array<glm::vec3, 4> colourVtx
    {
        glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f)
    };

    // Store vertex and colour info in VAO and bind to a VBO
    // -- Create VBO
    const GLsizeiptr	posBufferSize = sizeof(glm::vec2) * posVtx.size(),
                        colourBufferSize = sizeof(glm::vec3) * colourVtx.size();

    GLuint vboHandle;
    glCreateBuffers(1, &vboHandle);

    // allocate space for buffer storage
    // -- create data store on cpu RAM
    glNamedBufferStorage(vboHandle, posBufferSize + colourBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    // -- copy data from cpu RAM to gpu RAM
    glNamedBufferSubData(vboHandle, 0, posBufferSize, posVtx.data());
    glNamedBufferSubData(vboHandle, posBufferSize, colourBufferSize, colourVtx.data());

    #ifdef _DEBUG
    GLint maxAttribs, maxBindings;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxBindings);
    std::cout << "Maximum Vertex Attributes: " << maxAttribs << std::endl;
    std::cout << "Maxumim Buffer Binding Points: " << maxBindings << std::endl;
    #endif

    // Attribute indices
    constexpr GLuint posAttrib      = 0;
    constexpr GLuint colourAttrib   = 1;

    // Populate VAO
    // 1. Enable Attribute
    // 2. Bind VBO to binding point of the VAO
    // 3. Format Attribute Array to hold the correct data
    // 4. Bind Attribute to VBO
    glCreateVertexArrays(1, &vaoid);
    // -- Position Vertex Info
    /* 1 */ glEnableVertexArrayAttrib(vaoid, posAttrib);
    /* 2 */ glVertexArrayVertexBuffer(vaoid, 0, vboHandle, 0, sizeof(glm::vec2));
    /* 3 */ glVertexArrayAttribFormat(vaoid, posAttrib, 2, GL_FLOAT, GL_FALSE, 0);
    /* 4 */ glVertexArrayAttribBinding(vaoid, posAttrib, 0);
    // -- Colour Vertex Info
    /* 1 */ glEnableVertexArrayAttrib(vaoid, colourAttrib);
    /* 2 */ glVertexArrayVertexBuffer(vaoid, 1, vboHandle, posBufferSize, sizeof(glm::vec3));
    /* 3 */ glVertexArrayAttribFormat(vaoid, colourAttrib, 3, GL_FLOAT, GL_FALSE, 0);
    /* 4 */ glVertexArrayAttribBinding(vaoid, colourAttrib, 1);

    // Set up rectangle topology
    primitiveType = GL_TRIANGLES;
    // -- Set Vertices of triangle
    // Follow CCW direction
    std::array<GLushort, 6> idxVtx
    {
        0, 1, 2,	// First tri
        2, 3, 0		// Second tri
    };

    idxElemCnt = idxVtx.size();

    // -- allocate space for buffer storage
    // since data is laid out linearly, we can copy directly (no need to use glNamedBufferSubData)
    GLuint eboHandle;
    glCreateBuffers(1, &eboHandle);

    glNamedBufferStorage(eboHandle, sizeof(GLushort) * idxElemCnt, 
                         reinterpret_cast<GLvoid*>(idxVtx.data()), 
                         GL_DYNAMIC_STORAGE_BIT);
    // -- configure VAO with topology data
    glVertexArrayElementBuffer(vaoid, eboHandle);

    // Stop binding to prevent any more changes to VAO state
    glBindVertexArray(0);
}

void GLApp::GLModel::Setup_ShaderProgram()
{
    // Shader Program Flow
    // 1. Create Shader
    // 2. Get Vertex Shader Source Code from strings
    // 3. Compile Vertex Shader Source Code
    // 4. Create Shader Program
    // 5. Get Fragment Shader Source Code, attach to Shader Program
    // 6. Link Vertex & Fragment Shaders into Single Shader Program (Make sure to verify link)
    // 7. Install Shader Program on OpenGL pipeline using glUseProgram

    std::vector<std::pair<GLenum, std::string>> shaderFiles;

    std::pair<GLenum, std::string> vtxShader { GL_VERTEX_SHADER, "../shaders/tutorial-1.vert" };
    std::pair<GLenum, std::string> fragShader{ GL_FRAGMENT_SHADER, "../shaders/tutorial-1.frag" };

    shaderFiles.emplace_back(vtxShader);
    shaderFiles.emplace_back(fragShader);

    shaderProg.CompileLinkValidate(shaderFiles);

    if (shaderProg.IsLinked() == GL_FALSE)
    {
        std::cout << "Unable to compile/link/validate shader programs\n";
        std::cout << shaderProg.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void GLApp::GLModel::Draw()
{
    // Use the shader
    shaderProg.Use();

    // Use the VAO state to set up the pipeline
    glBindVertexArray(vaoid);
    // Provide information to graphics driver to render geometry
    glDrawElements(primitiveType, idxElemCnt, GL_UNSIGNED_SHORT, NULL);

    glBindVertexArray(0);
    shaderProg.UnUse();
}

/*-----------------------------------------------------------------------------*/
/* Helper Functions                                                            */
/*-----------------------------------------------------------------------------*/

/****************************************************************************//*!
@brief     Helper function to lerp between two colour values.

@param[in]    lerpState
     The current lerp direction.
@param[in]    lerpValue
     The current value while lerping.
@param[in]    lerpAmount
     The increment/decrement in each frame to adjust the lerpValue by.
*//*****************************************************************************/
void colourLerp(bool& lerpState, GLfloat& lerpValue, GLfloat lerpAmount)
{
    lerpValue += lerpState ? -lerpAmount : lerpAmount;

    if (lerpValue > 1.f && !lerpState)
    {
        lerpValue = 1.f;
        lerpState = true;
    }
    else if (lerpValue < 0.f && lerpState)
    {
        lerpValue = 0.f;
        lerpState = false;
    }
}

/****************************************************************************//*!
@brief     Helper function to store the attribute indices in a container.

@param[out]    attribIdxList
    A pointer to the container to store the attribute indices
@param[in]     maxAttribs
    A read-only reference to the maximum number of vertex attributes.
*//*****************************************************************************/
void buildAttribsArray(std::vector<GLint>& attribIdxList, const GLint& maxAttribs)
{
    for (GLint i = 0; i < maxAttribs; ++i) { attribIdxList.emplace_back(i); }
}

/****************************************************************************//*!
@brief     Helper function to store the binding point indices in a container.

@param[out]    bindingIdxList
    A pointer to the container to store the binding point indices
@param[in]     maxBindings
    A read-only reference to the maximum number of binding points
*//*****************************************************************************/
void buildBindingsArray(std::vector<GLint>& bindingIdxList, const GLint& maxBindings)
{
    for (GLint i = 0; i < maxBindings; ++i) { bindingIdxList.emplace_back(i); }
}

/****************************************************************************//*!
@brief     Helper function print information about the current openGL context
           to the CLI.
*//*****************************************************************************/
void printContextInfo()
{
    // Info vars
    GLint	majorVer, minorVer, doubleBuffered, 
            maxVtxCnt, maxIdxCnt, maxTexSize,
            maxViewportDims[2];

    // Populate info vars
    glGetIntegerv(GL_MAJOR_VERSION,			&majorVer);
    glGetIntegerv(GL_MINOR_VERSION,			&minorVer);
    glGetIntegerv(GL_DOUBLEBUFFER,			&doubleBuffered);
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,	&maxVtxCnt);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES,	&maxIdxCnt);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,		&maxTexSize);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS,		maxViewportDims);

    // Output info to CLI
    std::cout << "GPU Vendor: "			<< glGetString(GL_VENDOR)					<< std::endl;
    std::cout << "GL Renderer: "		<< glGetString(GL_RENDERER)					<< std::endl;
    std::cout << "GL Version: "			<< glGetString(GL_VERSION)					<< std::endl;
    std::cout << "GL Shader Version: "	<< glGetString(GL_SHADING_LANGUAGE_VERSION)	<< std::endl;

    std::cout << "GL Major Version: "	<< majorVer << std::endl;
    std::cout << "GL Minor Version: "	<< minorVer << std::endl;

    std::cout << "Current OpenGL Context " << (doubleBuffered ? "is" : "is not") << " double buffered\n";

    std::cout << "Maximum Vertex Count: "	<< maxVtxCnt  << std::endl;
    std::cout << "Maximum Indices Count: "	<< maxIdxCnt  << std::endl;
    std::cout << "GL Maximum texture size: "<< maxTexSize << std::endl;

    std::cout << "Maximum Viewport Dimension: " << maxViewportDims[0] << " x " << maxViewportDims[1] << std::endl;
}

/************************************************************************************//*!
\file           glapp.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 30, 2021
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

#include <chrono>
#include <random>   // std::default_random_generator

#include <glapp.h>
#include <glm/gtc/constants.hpp>    // glm::pi
#include <glm/gtc/type_ptr.hpp>     // glm::value_ptr

/*                                                                           constants
-------------------------------------------------------------------------------------- */

#define MDL_BOX         0
#define MDL_MYSTERY     1

const std::pair<float, float>   WINDOW_HALF_EXTENTS = { 1600.f * 0.5f, 900.f * 0.5f };
const std::pair<float, float>   SCALE_BOUNDS        = { 30.f, 70.f };
const std::pair<float, float>   ANGULAR_SPEED_BOUND = { -30.f, 30.f };
const std::pair<float, float>   ANGLE_LIMIT         = { -360.f, 360.f };

const int                       MAX_OBJS            = 32768;
constexpr const float           TWO_PI              = 2 * glm::pi<float>();

/*                                                             objects with file scope
-------------------------------------------------------------------------------------- */

// Definition for static member variables of GLApp
std::vector<GLApp::GLModel>     GLApp::models;
std::vector<GLSLShader>         GLApp::shaderPrograms;
std::list<GLApp::GLObject>      GLApp::objects;

// Tracking variables
static int      numBoxes;
static int      numMystery;
static GLenum   polygonMode;        // The polygon rendering mode of the scene
static bool     addObjects;         // Whether to increase the number of objects in the scene

/*                                                               function declarations
-------------------------------------------------------------------------------------- */
float   randFloat           (const float& lowerBound = 0.f, const float& upperBound = 1.f);
int     randInt             (const int& lowerBound = 0, const int& upperBound = 1);
void    changePolygonMode   (GLboolean& keyState);

/*                                                                function definitions
-------------------------------------------------------------------------------------- */

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions                                                                 */
/*-------------------------------------------------------------------------------------*/

void GLApp::init() 
{
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glViewport(0, 0, GLHelper::width, GLHelper::height);

    // Instance models in a container
    initModelsCont();
    // Shaders
    GLApp::VPSS shaderFileNames =
    {
        std::make_pair<std::string, std::string>
        (
            "../shaders/tutorial-3.vert",
            "../shaders/tutorial-3.frag"
        )
    };

    initShaderProgramsCont(shaderFileNames);

    // Set tracking variables
    numBoxes        = 0;
    numMystery      = 0;
    addObjects      = true;     // Increase objects by default
    polygonMode     = GL_FILL;  // Set to fill mode by default

    // Print current GPU info to CLI
    GLHelper::printGPUSpecs();
}

void GLApp::update(double delta_time) 
{
    if (GLHelper::leftMouseState == GL_TRUE)
    {
        size_t currObjectsSize = objects.size();

        if (addObjects)
        {
            // Add first object if container is empty
            if (objects.empty())
            {
                objects.emplace_back();
                objects.back().init();

                // Increment counter based on latest object model type
                switch (objects.back().modelRef)
                {
                    case MDL_BOX:
                    {
                        ++numBoxes;
                        break;
                    }
                    case MDL_MYSTERY:
                    {
                        ++numMystery;
                        break;
                    }
                }
            }

            // Double number of objects in the list
            // -- second condition to ensure objects never exceed max size (redundancy)
            for (size_t i = 0; i < currObjectsSize && objects.size() < MAX_OBJS; ++i)
            {
                objects.emplace_back();
                objects.back().init();

                switch (objects.back().modelRef)
                {
                    case MDL_BOX:
                    {
                        ++numBoxes;
                        break;
                    }
                    case MDL_MYSTERY:
                    {
                        ++numMystery;
                        break;
                    }
                }
            }
        }
        else
        {
            // Half the number of objects in the list
            for (size_t i = 0; i < currObjectsSize / 2; ++i)
            {
                // Decerement counter based on earliest object added
                switch (objects.front().modelRef)
                {
                    case MDL_BOX:
                    {
                        --numBoxes;
                        break;
                    }
                    case MDL_MYSTERY:
                    {
                        --numMystery;
                        break;
                    }
                }
                // Remove objects from the front
                objects.pop_front();
            }
        }

        // set mouse state to false to prevent any more objects from spawning in this frame
        GLHelper::leftMouseState = GL_FALSE;
    }

    switch (objects.size())
    {
        case MAX_OBJS:  // Reduce number of objects when max number is reached
        {
            addObjects = false;
            break;
        }
        case 1:         // Increase number of objects when 1 object is left
        {
            addObjects = true;
            break;
        }
    }

    // Set polygon mode
    changePolygonMode(GLHelper::keystateP);

    // Update all Objects
    for (auto& obj : objects) { obj.update(delta_time); }
}

void GLApp::draw()
{
    // Set title bar
    // ----------------------------------------------------------------------------------         
    std::stringstream sstr;
    sstr    << GLHelper::title
            << " | " << std::to_string(objects.size())  << " objects"
            << " | " << std::to_string(numBoxes)        << " boxes"
            << " | " << std::to_string(numMystery)      << " mystery";
            
    sstr << std::fixed << std::setprecision(2) << " | fps: " << GLHelper::fps;

    glfwSetWindowTitle(GLHelper::ptr_window, sstr.str().c_str());
    // ----------------------------------------------------------------------------------

    // Send back buffer information to GPU
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // Draw objects
    for (auto const& obj : GLApp::objects) { obj.draw(); }
}
 
void GLApp::cleanup()
{
    for (auto& mdl : models) { mdl.deleteHandles(); } // Delete VAO, VBO & EBO for models
    for (auto& prog : shaderPrograms) { glDeleteProgram(prog.GetHandle()); } // Delete Shader Programs
}

/*-------------------------------------------------------------------------------------*/
/* Member Functions																       */
/*-------------------------------------------------------------------------------------*/

void GLApp::initModelsCont()
{
    const int HEART_SLICES = 50;

    GLApp::models.emplace_back(boxModel());
    GLApp::models.emplace_back(mysteryModel(HEART_SLICES));
}

void GLApp::initShaderProgramsCont(VPSS const& vpss)
{
    for (auto const& x : vpss)
    {
        GLSLShader tmpProgram;
        std::vector<std::pair<GLenum, std::string>> shaderFiles;

        // Compile Shaders
        shaderFiles.emplace_back(std::make_pair(GL_VERTEX_SHADER, x.first));
        shaderFiles.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, x.second));

        tmpProgram.CompileLinkValidate(shaderFiles);

        // Insert shader program into container
        shaderPrograms.emplace_back(tmpProgram);
    }
}

GLApp::GLModel GLApp::boxModel()
{
    constexpr const int NUM_VTX = 4;

    std::array<glm::vec2, NUM_VTX> posVtx =
    {
        glm::vec2(-0.5f, -0.5f),    // 0
        glm::vec2(0.5f, -0.5f),     // 1
        glm::vec2(-0.5f, 0.5f),     // 2
        glm::vec2(0.5f, 0.5f)       // 3
    };

    std::array<glm::vec3, NUM_VTX> colourVtx =
    {
        glm::vec3(randFloat(), randFloat(), randFloat()),
        glm::vec3(randFloat(), randFloat(), randFloat()),
        glm::vec3(randFloat(), randFloat(), randFloat()),
        glm::vec3(randFloat(), randFloat(), randFloat())
    };

    std::array<GLushort, NUM_VTX> idxVtx = { 2, 0, 3, 1 };

    // Send data to GPU
    // ----------------------------------------------------------------------------------
    GLModel model(GL_TRIANGLE_STRIP);
    model.drawCount         = NUM_VTX;        // Num indices
    model.primitiveCount    = NUM_VTX - 2;    // Num primitives

    const GLsizeiptr POS_SIZE       = sizeof(glm::vec2) * NUM_VTX;
    const GLsizeiptr COLOUR_SIZE    = sizeof(glm::vec3) * NUM_VTX;
    const GLsizeiptr IDX_SIZE       = sizeof(GLushort)  * NUM_VTX;

    // Send VBO data to GPU
    glNamedBufferStorage(model.VBO, POS_SIZE + COLOUR_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(model.VBO, 0, POS_SIZE, posVtx.data());
    glNamedBufferSubData(model.VBO, POS_SIZE, COLOUR_SIZE, colourVtx.data());
    // Bind VBO to VAO
    BindVBOPoints(model.VAO, model.VBO);
    BindVBOColour(model.VAO, model.VBO, POS_SIZE);
    // Send EBO data to GPU
    glNamedBufferStorage(model.EBO, IDX_SIZE,
                         reinterpret_cast<GLvoid*>(idxVtx.data()), GL_DYNAMIC_STORAGE_BIT);
    // Bind EBO to VAO
    glVertexArrayElementBuffer(model.VAO, model.EBO);
    // Unbind VAO to prevent further changes
    glBindVertexArray(0);
    // ----------------------------------------------------------------------------------

    return model;
}

GLApp::GLModel GLApp::mysteryModel(const int& slices)
{
    const float DELTA = TWO_PI / slices;

    std::vector<glm::vec2> posVtx;
    std::vector<glm::vec3> colourVtx;

    // Populate position and colour vertex data
    for (float t = 0.f; t < TWO_PI; t += DELTA)
    {
        posVtx.emplace_back
        (
            (16.f * sin(t) * sin(t) * sin(t)) / 16.f,
            (13.f * cos(t) - 5.f * cos(2.f * t) - 2.f * cos(3.f * t) - cos(4.f * t)) / 13.f
        );

        // Selected range of red for the heart.
        colourVtx.emplace_back (randFloat(200.f / 255.f, 1.f), 0.f, 0.f);
    }

    // Send data to GPU
    // ----------------------------------------------------------------------------------
    GLModel model(GL_TRIANGLE_FAN);

    const GLsizeiptr POS_SIZE       = sizeof(glm::vec2) * posVtx.size();
    const GLsizeiptr COLOUR_SIZE    = sizeof(glm::vec3) * colourVtx.size();

    model.drawCount         = posVtx.size();        // Num vertices
    model.primitiveCount    = model.drawCount - 2;  // Num primitives

    glNamedBufferStorage(model.VBO, POS_SIZE + COLOUR_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(model.VBO, 0, POS_SIZE, posVtx.data());
    glNamedBufferSubData(model.VBO, POS_SIZE, COLOUR_SIZE, colourVtx.data());
    BindVBOPoints(model.VAO, model.VBO);
    BindVBOColour(model.VAO, model.VBO, POS_SIZE);
    glBindVertexArray(0);
    // ----------------------------------------------------------------------------------

    return model;
}

/*-------------------------------------------------------------------------------------*/
/* GLModel Member Functions                                                            */
/*-------------------------------------------------------------------------------------*/

void GLApp::GLModel::deleteHandles()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

/*-------------------------------------------------------------------------------------*/
/* GLObject Member Functions                                                           */
/*-------------------------------------------------------------------------------------*/

void GLApp::GLObject::init()
{
    modelRef    = randInt();    // Random number generator between 0 and 1
    shaderRef   = 0;            // Assume there is only 1 shader program for this tutorial.

    // Randomise transform data
    position            = glm::vec2(randFloat(-WINDOW_HALF_EXTENTS.first, WINDOW_HALF_EXTENTS.first),
                                    randFloat(-WINDOW_HALF_EXTENTS.second, WINDOW_HALF_EXTENTS.second));

    scale               = glm::vec2(randFloat(SCALE_BOUNDS.first, SCALE_BOUNDS.second),
                                    randFloat(SCALE_BOUNDS.first, SCALE_BOUNDS.second));

    angularSpeed        = randFloat(ANGULAR_SPEED_BOUND.first, ANGULAR_SPEED_BOUND.second);

    rotation            = randFloat(ANGLE_LIMIT.first, ANGLE_LIMIT.second);
}

void GLApp::GLObject::update(GLdouble dt)
{
    // NOTE: transpose matrix from row major to column major to fit GLSL uniforms
    rotation += static_cast<GLfloat>(angularSpeed * dt);
    float rotationRad = glm::radians(rotation);

    // Set matrix
    // -- Inverse scaling matrix from world to NDC
    modelToNDC =
    {
        1.f / WINDOW_HALF_EXTENTS.first, 0.f, 0.f,
        0.f, 1.f / WINDOW_HALF_EXTENTS.second, 0.f,
        0.f, 0.f, 1.f
    };
    // TRS Matrix (Row Major)
    glm::mat3 TRS = glm::transpose(glm::mat3
    (
        scale.x * cos(rotationRad), scale.y * -sin(rotationRad), position.x,
        scale.x * sin(rotationRad), scale.y * cos(rotationRad), position.y,
        0.f, 0.f, 1.f
    ));

    // Compute matrix
    modelToNDC *= TRS;
}

void GLApp::GLObject::draw() const
{
    GLModel     model           = models[modelRef];
    GLSLShader  shaderProgram   = shaderPrograms[shaderRef];

    // Setup
    shaderProgram.Use();            // install shader
    glBindVertexArray(model.VAO);   // bind VAO
    
    // Copy uniform 3x3 modelToNDC matrix to vertex shader
    shaderProgram.SetUniform("uModel_to_NDC", modelToNDC);

    switch (model.primitiveType)
    {
        case GL_TRIANGLE_FAN:
        {
            glDrawArrays(model.primitiveType, 0, model.drawCount);
            break;
        }
        case GL_TRIANGLE_STRIP:
        {
            glDrawElements(model.primitiveType, model.drawCount, GL_UNSIGNED_SHORT, NULL);
            break;
        }
    }

    // Clean Up
    glBindVertexArray(0);       // unbind VAO
    shaderProgram.UnUse();      // uninstall shader
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
/* Helper Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
@brief     Gets a random float between a specified range with uniform distribution.
           Generates a number between 0.f and 1.f by defualt.

@param[in]    lowerBound
    The lower bound of the randomised range
@param[in]    upperBound
    The upper bound of the randomised range
*//*************************************************************************************/
float randFloat(const float& lowerBound, const float& upperBound)
{
    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());

    std::default_random_engine generator(seed);

    std::uniform_real_distribution<float> distribution(lowerBound, upperBound);

    return distribution(generator);
}

/************************************************************************************//*!
@brief     Gets a random int between a specified range. Generates a random number
           between 0 and 1 by default.

@param[in]    lowerBound
    The lower bound of the randomised range
@param[in]    upperBound
    The upper bound of the randomised range
*//*************************************************************************************/
int randInt(const int& lowerBound, const int& upperBound)
{
    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());

    std::default_random_engine generator(seed);

    std::uniform_int_distribution<int> distribution(lowerBound, upperBound);

    return distribution(generator);
}

/************************************************************************************//*!
@brief     Gets a random int between a specified range. Generates a random number
           between 0 and 1 by default.

@param[in]    keyState
    The state of the key used to update the Polygon Rendering mode.
*//*************************************************************************************/
void changePolygonMode(GLboolean& keyState)
{
    if (keyState == GL_TRUE)
    {
        switch (polygonMode)
        {
            case GL_FILL:
            {
                polygonMode = GL_LINE;
                glLineWidth(5.f);
                break;
            }
            case GL_LINE:
            {
                polygonMode = GL_POINT;
                glPointSize(10.f);
                glLineWidth(1.f);
                break;
            }
            case GL_POINT:
            {
                polygonMode = GL_FILL;
                glPointSize(1.f);
                glLineWidth(1.f);
                break;
            }
        }

        keyState = GL_FALSE;
    }
}

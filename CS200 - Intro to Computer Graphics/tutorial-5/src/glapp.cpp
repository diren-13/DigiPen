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
#include <utility>  // std::pair

#include <glapp.h>

#include <glm/gtc/constants.hpp>    // glm::pi

#include <glutility.h>

/*                                                                           constants
-------------------------------------------------------------------------------------- */
const int       NUM_TASKS       = 7;

const double    ANIM_TIMER      = 30.0;
const double    MIN_TILE_SIZE   = 16.f;
const double    MAX_TILE_SIZE   = 256.f;

/*                                                             objects with file scope
-------------------------------------------------------------------------------------- */

// Definition for static member variables of GLApp
std::vector<GLModel>        GLApp::models;
std::vector<GLTexture>      GLApp::textures;
std::vector<GLSLShader>     GLApp::shaderPrograms;
Controller                  GLApp::controller(NUM_TASKS);

// Static variables 
static double   timer;              // timer for task 2 animation
static float    tileSize;           // uniform variable for task 1 and 2
static bool     animDirection;      // True for zooming in, false for zooming out

static std::pair<GLenum, GLenum>    textureAxis;        // Axis for the texture
static GLint                        textureMode;        // Mode for the texture

/*                                                               function declarations
-------------------------------------------------------------------------------------- */
double ease(double time);

/*                                                                function definitions
-------------------------------------------------------------------------------------- */

/*-------------------------------------------------------------------------------------*/
/* Lifecycle Functions                                                                 */
/*-------------------------------------------------------------------------------------*/

void GLApp::init() 
{
    // Use entire window as viewport
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glViewport(0, 0, GLHelper::width, GLHelper::height);

    // Initialise Models
    GLModel tempQuad;
    tempQuad.InitQuad();

    models.emplace_back(tempQuad);

    // Initialise Textures
    GLTexture duck;
    duck.Init("../images/duck-rgba-256.tex");

    textures.emplace_back(duck);
    
    // Initialse Shaders
    InsertShader("../shaders/tutorial-5.vert", "../shaders/tutorial-5.frag");

    // Default tileSize for task 1 & 2
    tileSize = 32.f;
    
    // task 2 variables
    timer           = 0.0;
    animDirection   = true;

    // texture variables
    textureAxis = { GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T };
    textureMode = GL_REPEAT;    // default mode is repeat

    // Print current GPU info to CLI
    GLHelper::printGPUSpecs();
}

void GLApp::update(double delta_time) 
{
    controller.Update(GLHelper::keystateT, GLHelper::keystateM, GLHelper::keystateA);

    // Fix tile size at 32 for task 1
    switch (controller.task)
    {
        case 1:     // Checkerboard pattern
        {
            tileSize = 32.f;
            break;
        }
        case 2:     // Checkerboard animation
        {
            if (timer > ANIM_TIMER || timer < 0.0) { animDirection = !animDirection; }
            timer += animDirection ? delta_time : -delta_time;

            double e = ease(timer / ANIM_TIMER);
            tileSize = static_cast<float>(MIN_TILE_SIZE + e * (MAX_TILE_SIZE - MIN_TILE_SIZE));
            break;
        }
    }
}

void GLApp::draw()
{
    // Set title bar
    // ----------------------------------------------------------------------------------         
    std::stringstream sstr;

    sstr << GLHelper::title; 
    sstr << " | Use T to change task | Use M to toggle modulate mode | Use A to toggle alpha blending";
    sstr << std::fixed << std::setprecision(2) << " | FPS: " << GLHelper::fps;
    

    glfwSetWindowTitle(GLHelper::ptr_window, sstr.str().c_str());
    // ----------------------------------------------------------------------------------

    // Send back buffer information to GPU
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the model
    // ----------------------------------------------------------------------------------
    glBindTextureUnit(0, textures[0].texObj);

    // Repeated Textures
    switch (controller.task)
    {
        case 4:     // Repeated Duck
        {
            textureMode = GL_REPEAT;
            break;
        }
        case 5:     // Repeated Mirrored Duck
        {
            textureMode = GL_MIRRORED_REPEAT;
            break;
        }
        case 6:     // Clamp to Edge Duck
        {
            textureMode = GL_CLAMP_TO_EDGE;
            break;
        }
        case 7:     // Clamp to Border
        {
            textureMode = GL_CLAMP_TO_BORDER;
            break;
        }
    }

    glTextureParameteri(textures[0].texObj, textureAxis.first, textureMode);
    glTextureParameteri(textures[0].texObj, textureAxis.second, textureMode);
    

    shaderPrograms[0].Use();

    // Enable/disbale blending based on controller
    if (controller.blendMode == true)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    // Send uniforms to shader
    // -- Controller
    shaderPrograms[0].SetUniform("uTask", controller.task);
    shaderPrograms[0].SetUniform("uModulateMode", controller.modulateMode);
    // -- Task Specific
    switch (controller.task)
    {
        case 1:     // Checkerboard
        case 2:     // Checkerboard animation
        {
            shaderPrograms[0].SetUniform("uTileSize", tileSize);
            break;
        }
        case 3:     // Duck
        {
            shaderPrograms[0].SetUniform("uTex2D", 0);
            break;
        }
    }

    glBindVertexArray(models[0].VAO);
    glDrawElements(models[0].primitiveType, models[0].drawCount, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);

    shaderPrograms[0].UnUse();
    // ---------------------------------------------------------------------------------- 
}
 
void GLApp::cleanup()
{
    // Return memory to GPU
    for (auto& model : models) { model.Release(); }
    for (auto& tex : textures) { tex.Release(); }
    
    // Delete shader programs
    for (auto& shader : shaderPrograms) { glDeleteProgram(shader.GetHandle()); }
}

/*-------------------------------------------------------------------------------------*/
/* Member Functions																       */
/*-------------------------------------------------------------------------------------*/

void GLApp::InsertShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    std::vector<std::pair<GLenum, std::string>> shaderFiles;
    // Compile Shaders
    shaderFiles.emplace_back(std::make_pair(GL_VERTEX_SHADER, vertexShader));
    shaderFiles.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, fragmentShader));

    GLSLShader shader;
    shader.CompileLinkValidate(shaderFiles);
    // Handle failure to link
    if (shader.IsLinked() == GL_FALSE)
    {
        std::cout << "Unable to compile/link/validate shader programs!" << std::endl;
        std::cout << shader.GetLog() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    shaderPrograms.emplace_back(shader);
}

/*-------------------------------------------------------------------------------------*/
/* Helper Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

/************************************************************************************//*!
@brief     Helper function for easing animation in task 2.

@param[in]    time
    The timer to calculate the ease value.
*//*************************************************************************************/
double ease(double time)
{
    // (sin(pi*time - pi/2) + 1) / 2
    return (glm::sin(glm::pi<double>() * time - glm::half_pi<double>()) + 1)* 0.5;
}
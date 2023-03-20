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

#include <glapp.h>
#include <glm/gtc/constants.hpp>    // glm::pi
#include <glm/gtc/type_ptr.hpp>     // glm::value_ptr

/*                                                                           constants
-------------------------------------------------------------------------------------- */
const float TWO_PI             = 2 * glm::pi<float>();
const int   SCENE_DATA_LINES   = 7;                     // number of lines per object in the scene file

/*                                                             objects with file scope
-------------------------------------------------------------------------------------- */

// Definition for static member variables of GLApp
std::map<std::string, GLModel>      GLApp::models;
std::map<std::string, GLSLShader>   GLApp::shaderPrograms;
std::map<std::string, GLObject>     GLApp::objects;
Camera2D                            GLApp::camera;

// Tracking variables
static GLenum   polygonMode;        // The polygon rendering mode of the scene

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
    // Use entire window as viewport
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glViewport(0, 0, GLHelper::width, GLHelper::height);

    // Initialise Scene and Camera
    InitScene("../scenes/tutorial-4.scn");
    camera.Init(GLHelper::ptr_window, &GLApp::objects.at("Camera"));

    polygonMode = GL_FILL;

    // Print current GPU info to CLI
    GLHelper::printGPUSpecs();
}

void GLApp::update(double delta_time) 
{
    // Update the camera first
    camera.Update(GLHelper::ptr_window, delta_time);

    for (auto& obj : objects)
    {
        if (&obj.second == camera.GetCameraObject()) { continue; }
        obj.second.update(delta_time, &camera);
    }

    changePolygonMode(GLHelper::keystateP);
}

void GLApp::draw()
{
    // Set title bar
    // ----------------------------------------------------------------------------------         
    std::stringstream sstr;

    sstr << GLHelper::title;        
    sstr << std::fixed << std::setprecision(2) << " | fps: " << GLHelper::fps;
    sstr << " | Camera Position: ("     << camera.GetCameraObject()->position.x << ", " << camera.GetCameraObject()->position.y << ")";
    sstr << std::fixed << std::setprecision(0) << " | Camera Orientation: "   << camera.GetCameraObject()->rotation << " degrees";
    sstr << " | Window Height: "        << camera.GetHeight();

    glfwSetWindowTitle(GLHelper::ptr_window, sstr.str().c_str());
    // ----------------------------------------------------------------------------------

    // Send back buffer information to GPU
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    for (const auto& obj : objects)
    {
        if (&obj.second == camera.GetCameraObject()) { continue; }
        obj.second.draw();
    }

    camera.GetCameraObject()->draw();
}
 
void GLApp::cleanup()
{
    // Return VBO to GPU
    for (auto& model : models) { model.second.Release(); }

    // Delete shader programs
    for (auto& shader : shaderPrograms) 
    { 
        glDeleteProgram(shader.second.GetHandle());
    }
}

/*-------------------------------------------------------------------------------------*/
/* Member Functions																       */
/*-------------------------------------------------------------------------------------*/

void GLApp::InsertShaderProgram(const std::string& programHandle, 
                                const std::string& vertexShader, 
                                const std::string& fragmentShader)
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

    // Map handle to compiled & linked shader
    shaderPrograms[programHandle] = shader;
}

void GLApp::InsertModel(const std::string& modelName)
{
    GLModel model;
    model.Init(modelName);

    // Map model to model name
    models[modelName] = model;
}

void GLApp::InitScene(std::string sceneFilename)
{
    std::ifstream ifs(sceneFilename, std::ios::in);

    // If file fails to open
    if (!ifs)
    {
        std::cout << "ERROR: Unable to open scene file: " << sceneFilename << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Set file cursor to beginnning of the file
    ifs.seekg(0, std::ios::beg);

    int         numObjs;
    std::string line;
    // Variables to be read in each block
    std::string modelName, objectName, shaderName;
    std::string vertexShaderPath, fragmentShaderPath;

    // First line in file is number of objects in scene
    std::getline(ifs, line);
    std::istringstream firstLine(line);  
    firstLine >> numObjs;

    while (numObjs--)
    {
        GLObject obj;

        for (int i = 0; i < SCENE_DATA_LINES; ++i)  // Use i as line number per object data
        {
            std::getline(ifs, line);
            std::istringstream inputData(line);

            switch (i)
            {
                case 0: // Model name
                {
                    inputData >> modelName;

                    // Check if model has already been instantiated
                    if (models.count(modelName) == 0)
                    {
                        // Add model to the container of models
                        InsertModel(modelName);
                    }

                    // Set object's model reference
                    obj.modelRef = models.find(modelName);
                    break;
                }
                case 1: // Object name
                {
                    inputData >> objectName;
                    break;
                }
                case 2: // Shader Name & Details
                {
                    inputData >> shaderName;
                    // Check if shader program has already been instantiated
                    if (shaderPrograms.count(shaderName) == 0)
                    {
                        inputData >> vertexShaderPath >> fragmentShaderPath;
                        InsertShaderProgram(shaderName, vertexShaderPath, fragmentShaderPath);
                    }
                    // Set object's shader reference
                    obj.shaderRef = shaderPrograms.find(shaderName);
                    break;
                }
                case 3: // Colour
                {
                    inputData >> obj.colour.r >> obj.colour.g >> obj.colour.b;
                    break;
                }
                case 4: // Scale (X, Y)
                {
                    inputData >> obj.scale.x >> obj.scale.y;
                    break;
                }
                case 5: // Rotation & Rotation Speed
                {
                    inputData >> obj.rotation >> obj.rotationSpeed;
                    break;
                }
                case 6: // Position
                {
                    inputData >> obj.position.x >> obj.position.y;
                    break;
                }
            }
        }

        objects[objectName] = obj;
    }

    ifs.close();
}

/*-------------------------------------------------------------------------------------*/
/* Helper Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

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

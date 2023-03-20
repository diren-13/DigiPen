/************************************************************************************//*!
\file           glhelper.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 30, 2021
\brief          This file implements functionality useful and necessary to build OpenGL
                applications including use of external APIs such as GLFW to create a
                window and start up an OpenGL context and use GLEW to extract function 
                pointers to OpenGL implementations.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

/*                                                                            includes
-------------------------------------------------------------------------------------- */
#include <glhelper.h>
#include <iostream>
#include <sstream>
#include <cstdlib>

/*                                                             objects with file scope
-------------------------------------------------------------------------------------- */
// static data members declared in GLHelper
GLint       GLHelper::width;
GLint       GLHelper::height;
GLdouble    GLHelper::fps;
std::string GLHelper::title;
GLFWwindow* GLHelper::ptr_window;

GLboolean   GLHelper::leftMouseState;
GLboolean   GLHelper::keystateT;
GLboolean   GLHelper::keystateM;
GLboolean   GLHelper::keystateA;

bool GLHelper::init(GLint w, GLint h, std::string t) 
{
    width = w;
    height = h;
    title = t;

    // Button states
    leftMouseState  = GL_FALSE;
    keystateT       = GL_FALSE;
    keystateM       = GL_FALSE;
    keystateA       = GL_FALSE;

    // Part 1
    if (!glfwInit()) 
    {
        std::cout << "GLFW init has failed - abort program!!!" << std::endl;
        return false;
    }

    // In case a GLFW function fails, an error is reported to callback function
    glfwSetErrorCallback(GLHelper::error_cb);

    // Before asking GLFW to create an OpenGL context, we specify the minimum constraints
    // in that context:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // window dimensions are static

    ptr_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!ptr_window) 
    {
        std::cerr << "GLFW unable to create OpenGL context - abort program\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(ptr_window);

    glfwSetFramebufferSizeCallback(ptr_window, fbsize_cb);
    glfwSetKeyCallback(ptr_window, key_cb);
    glfwSetMouseButtonCallback(ptr_window, mousebutton_cb);
    glfwSetCursorPosCallback(ptr_window, mousepos_cb);
    glfwSetScrollCallback(ptr_window, mousescroll_cb);

    // this is the default setting ...
    glfwSetInputMode(ptr_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Part 2: Initialize entry points to OpenGL functions and extensions
    GLenum err = glewInit();
    if (GLEW_OK != err) 
    {
        std::cerr   << "Unable to initialize GLEW - error: "
                    << glewGetErrorString(err) << " abort program" 
                    << std::endl;
        return false;
    }

    if (GLEW_VERSION_4_5) 
    {
        std::cout << "Using glew version: " << glewGetString(GLEW_VERSION) << std::endl;
        std::cout << "Driver supports OpenGL 4.5\n" << std::endl;
    } 
    else 
    {
        std::cerr << "Driver doesn't support OpenGL 4.5 - abort program" << std::endl;
        return false;
    }

    return true;
}


void GLHelper::cleanup() 
{
    // Part 1
    glfwTerminate();
}

void GLHelper::key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod) 
{
    switch (action)
    {
        case GLFW_PRESS:
        {
            #ifdef _DEBUG
            std::cout << "Key pressed" << std::endl;
            #endif

            // Esc key to close window
            if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(pwin, GLFW_TRUE); }
            // Letter keys
            keystateT = (key == GLFW_KEY_T);
            keystateM = (key == GLFW_KEY_M);
            keystateA = (key == GLFW_KEY_A);
            break;
        }
        case GLFW_REPEAT:
        {
            #ifdef _DEBUG
            std::cout << "Key repeatedly pressed" << std::endl;
            #endif

            // Letter Keys
            keystateT = GL_FALSE;
            keystateM = GL_FALSE;
            keystateA = GL_FALSE;

            break;
        }
        case GLFW_RELEASE:
        {
            #ifdef _DEBUG
            std::cout << "Key released" << std::endl;
            #endif

            // Letter Keys
            keystateT = GL_FALSE;
            keystateM = GL_FALSE;
            keystateA = GL_FALSE;

            break;
        }
    }
}


void GLHelper::mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod) 
{
    switch (button) 
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            #ifdef _DEBUG
            std::cout << "Left mouse button ";
            #endif
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            #ifdef _DEBUG
            std::cout << "Right mouse button ";
            #endif
            break;
        }
    }

    switch (action) 
    {
        case GLFW_PRESS:
        {
            #ifdef _DEBUG
            std::cout << "pressed!!!" << std::endl;
            #endif

            // left mouse
            leftMouseState = (button == GLFW_MOUSE_BUTTON_LEFT);

            break;
        }
        case GLFW_REPEAT:
        {
            #ifdef _DEBUG
            std::cout << "Key repeatedly pressed" << std::endl;
            #endif

            // left mouse
            leftMouseState = GL_FALSE;

            break;
        }
        case GLFW_RELEASE:
        {
            #ifdef _DEBUG
            std::cout << "released!!!" << std::endl;
            #endif

            // left mouse
            leftMouseState = GL_FALSE;

            break;
        }
    }
}


void GLHelper::mousepos_cb(GLFWwindow *pwin, double xpos, double ypos) 
{
    //#ifdef _DEBUG
    //std::cout << "Mouse cursor position: (" << xpos << ", " << ypos << ")" << std::endl;
    //#endif
}


void GLHelper::mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset) 
{
    //#ifdef _DEBUG
    //std::cout   << "Mouse scroll wheel offset: ("
    //            << xoffset << ", " << yoffset << ")" << std::endl;
    //#endif
}


void GLHelper::error_cb(int error, char const* description) 
{
    std::cerr << "GLFW error: " << description << std::endl;
}


void GLHelper::fbsize_cb(GLFWwindow *ptr_win, int width, int height) 
{
    std::cout << "fbsize_cb getting called!!!" << std::endl;
    // use the entire framebuffer as drawing region
    glViewport(0, 0, width, height);
    // later, if working in 3D, we'll have to set the projection matrix here ...
}

double GLHelper::update_time(double fps_calc_interval) 
{
    // get elapsed time (in seconds) between previous and current frames
    static double prev_time = glfwGetTime();
    double curr_time = glfwGetTime();
    double delta_time = curr_time - prev_time;
    prev_time = curr_time;

    // fps calculations
    static double count = 0.0; // number of game loop iterations
    static double start_time = glfwGetTime();
    // get elapsed time since very beginning (in seconds) ...
    double elapsed_time = curr_time - start_time;

    ++count;

    // update fps at least every 10 seconds ...
    fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
    fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;

    if (elapsed_time > fps_calc_interval)
    {
        GLHelper::fps = count / elapsed_time;
        start_time = curr_time;
        count = 0.0;
    }

    // done calculating fps ...
    return delta_time;
}

void GLHelper::printGPUSpecs()
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

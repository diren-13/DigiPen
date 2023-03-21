/*!
@file    main.cpp
@author  diren.dbharwani@digipen.edu
@date    13/5/2021

This file uses functionality defined in types GLHelper and GLApp to initialize 
an OpenGL context and implement a game loop.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
// Extension loader library's header must be included before GLFW's header!!!
#include <glhelper.h>
#include <glapp.h>
#include <iostream>
#include <sstream>
#include <iomanip>

/*                                                          type declarations
----------------------------------------------------------------------------- */

/*                                                      function declarations
----------------------------------------------------------------------------- */
static void draw();
static void update();
static void init();
static void cleanup();

/*                                                      function definitions
----------------------------------------------------------------------------- */
/*  _________________________________________________________________________ */
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main() 
{
    // Part 1
    init();

    // Part 2
    while (!glfwWindowShouldClose(GLHelper::ptr_window)) 
    {
        // Part 2a
        update();
        // Part 2b
        draw();
    }

    // Part 3
    cleanup();
}

/*  _________________________________________________________________________ */
/*! init
@param none
@return none

Get handle to OpenGL context through GLHelper::GLFWwindow*.
*/
static void init() 
{
    // Part 1
    if (!GLHelper::init(1600, 900, "Tutorial 3")) 
    {
        std::cout << "Unable to create OpenGL context" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Part 2
    GLApp::init();
}

/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
For now, there are no objects to animate nor keyboard, mouse button click,
mouse movement, and mouse scroller events to be processed.
*/
static void update() 
{
    // Part 1
    glfwPollEvents();

    // Part 2
    // time between previous and current frame
    double delta_time = GLHelper::update_time(1.0);
    // write window title with current fps ...
    //std::stringstream sstr;
    //sstr << std::fixed << std::setprecision(2) << GLHelper::title << ": " << GLHelper::fps;
    //glfwSetWindowTitle(GLHelper::ptr_window, sstr.str().c_str());

    // Part 3
    GLApp::update(delta_time);
}

/*  _________________________________________________________________________ */
/*! draw
@param none
@return none

Uses GLHelper::GLFWWindow* to get handle to OpenGL context.
For now, there's nothing to draw - just paint color buffer with constant color
*/
static void draw() 
{
    // Part 1
    GLApp::draw();

    // Part 2: swap buffers: front <-> back
    glfwSwapBuffers(GLHelper::ptr_window);
}



/*  _________________________________________________________________________ */
/*! cleanup
@param none
@return none

Return allocated resources for window and OpenGL context thro GLFW back
to system.
Return graphics memory claimed through 
*/
void cleanup() 
{
    // Part 1
    GLHelper::cleanup();
    // Part 2
    GLApp::cleanup();
}

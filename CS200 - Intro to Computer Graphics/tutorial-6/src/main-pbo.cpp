/************************************************************************************//*!
\file           main-pbo.cpp
\author         Prasanna Ghali
\par            email: pghali\@digipen.edu
\date           October 11, 2016
\brief          This file uses functionality defined in types GLHelper, GLApp, and GLPbo 
                to initialize an OpenGL context and implement a game loop.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#include <glhelper.h>
#include <glpbo.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>


static void init();
static void update();
static void draw();
static void cleanup();


int main() 
{
    init();

    while (!glfwWindowShouldClose(GLHelper::ptr_window)) 
    {
        update();
        draw();
    }

    cleanup();
}


static void init()
{
    if (!GLHelper::init(1600, 900, "CSD2100 Lab 6: Setting up GPU rasterization stage emulator"))
    {
        std::cout << "Unable to create OpenGL context" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    GLPbo::init(GLHelper::width, GLHelper::height);
}


static void update() 
{
    glfwPollEvents();

    double delta_time = GLHelper::update_time(1.0);
    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(2) << GLHelper::title << ": " << GLHelper::fps;
    glfwSetWindowTitle(GLHelper::ptr_window, sstr.str().c_str());

    GLPbo::emulate();
}


static void draw() 
{
    GLPbo::draw_fullwindow_quad();

    glfwSwapBuffers(GLHelper::ptr_window);
}


void cleanup() 
{
    GLPbo::cleanup();
    GLHelper::cleanup();
}

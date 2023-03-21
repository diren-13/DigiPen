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

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


static void init();
static void update();
static void draw();
static void cleanup();


int main() 
{
    #ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    //_CrtSetBreakAlloc(/*Allocation Number here*/);    // Uncomment to cause a break on allocation for debugging
    #endif

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
    if (!GLHelper::init(1600, 900, "CSD2100"))
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

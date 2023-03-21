/************************************************************************************//*!
\file           glhelper.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 30, 2021
\brief          This file contains the declaration of namespace Helper that encapsulates the
                functionality required to create an OpenGL context using GLFW; use GLEW
                to load OpenGL extensions; initialize OpenGL state; and finally initialize
                the OpenGL application by calling initalization functions associated with
                objects participating in the application.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

/*                                                                               guard
-------------------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                            includes
-------------------------------------------------------------------------------------- */
#include <GL/glew.h>        // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <string>

/************************************************************************************//*!
@brief    Encapsulates a helper class to interface with the OpenGL context.
*//*************************************************************************************/
struct GLHelper
{
    /*---------------------------------------------------------------------------------*/
    /* Member Variables                                                                */
    /*---------------------------------------------------------------------------------*/
    static GLint		width;			// the width of the OpenGL context window
    static GLint		height;			// the height of the OpenGL context window
    static GLdouble		fps;			// The current framerate per second of the application
    static std::string	title;			// The title string of the OpenGL context window
    static GLFWwindow*  ptr_window;	    // A pointer to an OpenGL context window

    // Key states
    static GLboolean	leftMouseState;	// The state of the left mouse button.
    static GLboolean    keystateT;      // The state of the 'T' key.
    static GLboolean    keystateM;      // The state of the 'M' key.
    static GLboolean    keystateA;      // The state of the 'A' key.

    /*---------------------------------------------------------------------------------*/
    /* Member Functions															       */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief     Uses GLFW to create OpenGL context. GLFW's initialization follows from 
               here: http://www.glfw.org/docs/latest/quick.html
               A window of size width x height pixels
               and its associated OpenGL context that matches a core profile that is 
               compatible with OpenGL 4.5 and doesn't support "old" OpenGL, 
               has 32-bit RGBA, double-buffered color buffer, 24-bit depth buffer 
               and 8-bit stencil buffer with each buffer of size width x height pixels.

    @param[in]	  w
        The width of the context window.
    @param[in]	  h
        The height of the context window.
    @param[in]	  t
        The title for the context window.

    @returns    If the OpenGL has successfully been initialised.
    *//*********************************************************************************/
    static bool init(GLint w, GLint h, std::string t);

    /********************************************************************************//*!
    @brief     Cleans up & deallocates resources for the OpenGL context.
    *//*********************************************************************************/
    static void cleanup();
  
    /********************************************************************************//*!
    @brief     The error callback receives a human-readable description of the error
               (when possible) its cause.

    @param[in]	  error
        The width of the context window.
    @param[in]	  description
        The height of the context window.
    *//*********************************************************************************/
    static void error_cb(int error, char const* description);

    /****************************************************************************//*!
    @brief     This function is called when the window is resized - it receives 
               the new size of the window in pixels.

    @param[in]	  ptr_win
       A pointer to an OpenGL context window.
    @param[in]	  width
        The width of the context window.
    @param[in]    height
        The height of the context window.
    *//*********************************************************************************/
    static void fbsize_cb(GLFWwindow *ptr_win, int width, int height);
    
    /********************************************************************************//*!
    @brief     This function is called when keyboard buttons are pressed.
               When the ESC key is pressed, the close flag of the window is set.

    @param[in]	  pwin
        A pointer to the window of the current OpenGL context.
    @param[in]	  key
        The keyboard key that was pressed or released.
    @param[in]    scancode
        The platform-specific scancode of the key.
    @param[in]    action
        GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
        action will be GLFW_KEY_UNKNOWN if GLFW lacks a key token for it,
        for example E-mail and Play keys.
    @param[in]    mod
        Bit-field describing which modifier keys (SHIFT, ALT, CONTROL)
        were held down.
    *//*********************************************************************************/
    static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);

    /********************************************************************************//*!
    @brief     This function is called when mouse buttons are pressed.

    @param[in]	  pwin
        A pointer to the window of the current OpenGL context.
    @param[in]	  button
        GLFW_MOUSE_BUTTON_LEFT and GLFW_MOUSE_BUTTON_RIGHT specifying left and right
        mouse buttons are most useful.
    @param[in]    action
        GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
        action will be GLFW_KEY_UNKNOWN if GLFW lacks a key token for it,
        for example E-mail and Play keys.
    @param[in]    mod
        Bit-field describing which modifier keys (SHIFT, ALT, CONTROL)
        were held down.
    *//*********************************************************************************/
    static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);

    /********************************************************************************//*!
    @brief     This function is called when the user scrolls, whether with a mouse 
               wheel or touchpad gesture. Although the function receives 2D scroll 
               offsets, a simple mouse scroll wheel, being vertical, provides offsets 
               only along the Y-axis.

    @param[in]	  pwin
        A pointer to the window of the current OpenGL context.
    @param[in]	  xoffset
        Scroll offset along X-axis.
    @param[in]    xoffset
        Scroll offset along Y-axis.
    *//*********************************************************************************/
    static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);

    /********************************************************************************//*!
    @brief     This functions receives the cursor position, measured in screen 
               coordinates but relative to the top-left corner of the window client 
               area.

    @param[in]	  pwin
        A pointer to the window of the current OpenGL context.
    @param[in]	  xpos
        New cursor x-coordinate, relative to the left edge of the client area.
    @param[in]    ypos
        New cursor y-coordinate, relative to the top edge of the client area.
    *//*********************************************************************************/
    static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);

    /********************************************************************************//*!
    @brief     This function is first called in init() and once each game loop by 
               update(). It uses GLFW's time functions to:
               1. compute interval in seconds between each frame
               2. compute the frames per second every "fps_calc_interval" seconds

    @param[in]	  pwin
        A pointer to the window of the current OpenGL context.
    @param[in]	  fpsCalcInt
        The interval (in seconds) at which fps is to be calculated.

    @returns    The delta time: the interval between each frame.
    *//*********************************************************************************/
    static double update_time(double fpsCalcInt = 1.0);

    /********************************************************************************//*!
    @brief     Function to print the GPU specs of the system to the CLI.
    *//*********************************************************************************/
    static void printGPUSpecs();
};

#endif /* GLHELPER_H */

/*+======================================================================================
File:       Application.h

Date:       19th January 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for the entire application

Classes:    RTR::Application
======================================================================================+*/

#pragma once

// Standard Libraries
#include <Windows.h>
#include <vector>
// Project Headers
#include "Window.h"
#include "Config.h"


namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates the application.
    /// </summary>
    class Application
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor for application.
        /// </summary>
        /// <param name="_config">
        /// The configuration properties for the application.
        /// </param>
        /// <param name="_windowInstance">
        /// The handle to an instance of the native window.
        /// </param>
        Application(const Config& _config, HINSTANCE _windowInstance);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        /// <summary>
        /// Runs the application.
        /// </summary>
        void Run();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Config                      config;
        Window                      window;
        std::string                 appName;
    };

} // namespace RTR

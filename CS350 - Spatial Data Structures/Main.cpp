/************************************************************************************//*!
\file           main.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Entry point for the application
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Project Headers
#include "projects/Project2.h"
#include "projects/Project3.h"

#define CURRENT_PROJECT 3

int APIENTRY wWinMain
(
    _In_        HINSTANCE   ,
    _In_opt_    HINSTANCE   ,
    _In_        LPWSTR      ,
    _In_        int         
)
{
    // Check for memory leaks
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
        //_CrtSetBreakAlloc( /* Insert allocation number here */);
    }

    try
    {
        if (CURRENT_PROJECT == 2)
        {
            Project2 project2App;

            project2App.Initialise();
            project2App.Update();
            project2App.Exit();
        }

        if (CURRENT_PROJECT == 3)
        {
            Project3 project3App;

            project3App.Initialise();
            project3App.Update();
            project3App.Exit();
        }

        
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Application crashed with error: \n" << e.what();
        CC::Log(CC::LogSeverity::Error, ss.str().c_str());
    }
    

    return EXIT_SUCCESS;
}
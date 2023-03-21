/************************************************************************************//*!
\file           Console.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Definitions for functions declared in the singleton Console.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/


// Precompiled Header
#include "pch.h"
// Primary Header
#include "Tools/Console.h"
// STL Headers
#include <cstdio>
#include <iostream>
#include <iomanip>          // std::put_time
#include <sstream>
// Windows Header
#include <Windows.h>

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    Console Console::console;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Console::Console()
    {
        // Attempt to create new console
        if (AllocConsole())
        {
            adjustConsoleBuffer(1024U);
            redirectConsoleIO();
        }
    }

    Console::~Console()
    {
        bool result = true;
        FILE* fP;

        // Just to be safe, redirect standard IO to NUL before releasing.

        // Redirect STDIN to NUL
        if (freopen_s(&fP, "NUL:", "r", stdin) != 0)
        {
            result = false;
        }
        else
        {
            setvbuf(stdin, nullptr, _IONBF, 0);
        }
            

        // Redirect STDOUT to NUL
        if (freopen_s(&fP, "NUL:", "w", stdout) != 0)
        {
            result = false;
        }
        else
        {
            setvbuf(stdout, nullptr, _IONBF, 0);
        }

        // Redirect STDERR to NUL
        if (freopen_s(&fP, "NUL:", "w", stderr) != 0)
        {
            result = false;
        }
        else
        {
            setvbuf(stderr, nullptr, _IONBF, 0);
        }

        // Detach console
        FreeConsole();
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Member Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    bool Console::redirectConsoleIO()
    {
        bool result = true;
        FILE* fp;

        // Redirect STDIN if the console has an input handle
        if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
        {
            if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
            {
                result = false;
            }
                
            else
            {
                setvbuf(stdin, nullptr, _IONBF, 0);
            }
        }

        // Redirect STDOUT if the console has an output handle
        if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
        {
            if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
            {
                result = false;
            }
            else
            {
                setvbuf(stdout, nullptr, _IONBF, 0);
            }
        }

        // Redirect STDERR if the console has an error handle
        if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
        {
            if (freopen_s(&fp, "CONOUT$", "w", stderr) != 0)
            {
                result = false;
            }
            else
            {
                setvbuf(stderr, nullptr, _IONBF, 0);
            }
        }

        // Make C++ standard streams point to console as well.
        std::ios::sync_with_stdio(true);

        // Clear the error state for each of the C++ standard streams.
        std::wcin.clear();
        std::cin.clear();
        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();

        return result;
    }

    void Console::adjustConsoleBuffer(unsigned short minLength)
    {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        // Set the screen buffer to be big enough to scroll some text
        CONSOLE_SCREEN_BUFFER_INFO conInfo;
        GetConsoleScreenBufferInfo(consoleHandle, &conInfo);

        if (conInfo.dwSize.Y < static_cast<short>(minLength))
        {
            conInfo.dwSize.Y = static_cast<short>(minLength);
        }
            
        SetConsoleScreenBufferSize(consoleHandle, conInfo.dwSize);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Log(LogSeverity severity, const std::string_view& msg)
    {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        WORD consoleColour = 0;
        switch (severity)
        {
            case LogSeverity::Info     :
            {
                consoleColour = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
                break;
            }
            case LogSeverity::Warning  :
            {
                consoleColour = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
                break;
            }
            case LogSeverity::Error    :
            {
                consoleColour = FOREGROUND_RED | FOREGROUND_INTENSITY;
                break;
            }
        }

        SetConsoleTextAttribute(consoleHandle, consoleColour);

        // Get time
        tm localTime;
        const auto NOW = std::time(0);
        localtime_s(&localTime, &NOW);

        const bool ERR = severity == LogSeverity::Error;
        (ERR ? std::cerr : std::cout) << std::put_time(&localTime, "[%D %T] ") << msg << std::endl;

        // Reset console to previous original colour
        const WORD CONSOLE_DEFAULT_COLOUR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        SetConsoleTextAttribute(consoleHandle, CONSOLE_DEFAULT_COLOUR);
    }

    void LogProgress(const std::string_view& msg, float progress)
    {
        static std::string_view lastMsg;

        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        GetConsoleScreenBufferInfo(consoleHandle, &bufferInfo);

        // Erase current line
        std::cout << std::string(lastMsg.length(), '\b');

        // Reset x
        bufferInfo.dwCursorPosition.X = 0;
        SetConsoleCursorPosition(consoleHandle, bufferInfo.dwCursorPosition);

        const WORD CONSOLE_DEFAULT_COLOUR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        SetConsoleTextAttribute(consoleHandle, CONSOLE_DEFAULT_COLOUR);

        std::cout << msg << std::setw(7) << std::setprecision(3) << (progress * 100.0f) << "%";

        lastMsg = msg;
    }


    void LogTitleBlock(const std::string_view& title)
    {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        const WORD CONSOLE_DEFAULT_COLOUR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        SetConsoleTextAttribute(consoleHandle, CONSOLE_DEFAULT_COLOUR);

        std::cout   << "-----------------------------------------------------------------------------------------" << '\n'
                    << title << '\n'
                    << "-----------------------------------------------------------------------------------------"
                    << std::endl;
    }

    void LogSeparator()
    {
        std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
    }




} // namespace CatEye
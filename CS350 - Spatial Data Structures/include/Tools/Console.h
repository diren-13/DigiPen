/************************************************************************************//*!
\file           Console.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Declarations for functions to create and destroy a console.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Windows Headers
#include "Window/WinConfig.h"
// Standard Libraries
#include <iostream>
#include <string_view>

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief  Singleton for a Console.
    *//*********************************************************************************/
    class Console
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] static const Console& GetConsole() { return console; }

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        static Console console;

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Console     ();
        ~Console    ();

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static bool redirectConsoleIO   ();
        static void adjustConsoleBuffer (unsigned short);
    };

    /********************************************************************************//*!
    @brief  Encapsulates severity levels of messages for logging.
    *//*********************************************************************************/
    enum class LogSeverity
    {
        Info
    ,   Warning
    ,   Error
    };

    void Log            (LogSeverity severity, const std::string_view& msg);
    void LogProgress    (const std::string_view& msg, float progress);
    void LogTitleBlock  (const std::string_view& title);
    void LogSeparator   ();

} // namespace CatEye

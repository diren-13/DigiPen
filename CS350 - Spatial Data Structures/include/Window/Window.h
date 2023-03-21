/************************************************************************************//*!
\file           Window.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of a Native Window.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Windows Headers
#include "WinConfig.h"
// Standard Libraries
#include <string>

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief    Encapsulates a Native Window.
    *//*********************************************************************************/
    class Window
    {
    private:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static int              count;
        static LPCSTR           CLASS_NAME;

        static constexpr int    DEFAULT_WIDTH   = 1280;
        static constexpr int    DEFAULT_HEIGHT  = 720;

    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        Window              (int Width = DEFAULT_WIDTH, int Height = DEFAULT_HEIGHT, const std::string& title = CLASS_NAME);
        ~Window             ();

        Window              (const Window&) = delete;
        Window              (Window&&)      = delete;
        Window& operator=   (const Window&) = delete;
        Window& operator=   (Window&&)      = delete;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] HWND  GetHandle       ()  const   { return handle; }
        
        [[nodiscard]] int   GetWidth        ()  const   { return width; }
        [[nodiscard]] int   GetHeight       ()  const   { return height; }
        [[nodiscard]] float GetAspectRatio  ()  const   { return static_cast<float>(width) / static_cast<float>(height); }

        [[nodiscard]] bool  GetResized      ()  const   { return resized; }
        [[nodiscard]] bool  GetMinimised    ()  const   { return minimised; }


        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        bool Update();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr long long  PREV_KEY_STATE_BIT = 0x40000000;

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        HWND    handle;

        int     width;
        int     height;

        bool    resized;
        bool    minimised;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static LRESULT  WndProc         (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        LRESULT         handleMessage   (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT         handleKeyboard  (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        LRESULT         handleMouse     (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    };
}



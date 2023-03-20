/*+======================================================================================
File:       Window.h

Date:       19th January 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for creating a window.

Classes:    RTR::Window
=========================================================================================+*/

#pragma once

// Standard Libraries
#include <Windows.h>
// External Dependencies
#include <vulkan/vulkan.hpp>
// Project Headers

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Forward Declarations                                                            */
    /*---------------------------------------------------------------------------------*/
    class SwapChain;

    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Window with an attached Console
    /// </summary>
    class Window
    {
    private:
        /*-----------------------------------------------------------------------------*/
        /* Constants                                                                   */
        /*-----------------------------------------------------------------------------*/
        static constexpr int DEFAULT_WINDOW_WIDTH   = 1920;
        static constexpr int DEFAULT_WINDOW_HEIGHT  = 1080;

    public:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static int MouseXPos;
        static int MouseYPos;
        static int MousePrevXPos;
        static int MousePrevYPos;

        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Constructor for a Window
        /// </summary>
        /// <param name="hInstance">
        /// The handle to the native window instance.
        /// </param>
        /// <param name="appName">
        /// The name of the application.
        /// </param>
        /// <param name="width">
        /// The width of the window. Defaults to 1920.
        /// </param>
        /// <param name="height">
        /// The height of the window. Defaults to 1080.
        /// </param>
        Window
        (
            HINSTANCE   hInstance, 
            LPCWSTR     appName, 
            int         width   = DEFAULT_WINDOW_WIDTH, 
            int         height  = DEFAULT_WINDOW_HEIGHT
        );

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const HWND&       GetWindow()         const { return window; }
        [[nodiscard]] const HINSTANCE&  GetWindowInstance() const { return windowInstance; }
        [[nodiscard]] uint32_t          GetWindowWidth()    const { return windowWidth; }
        [[nodiscard]] uint32_t          GetWindowHeight()   const { return windowHeight; }
        [[nodiscard]] bool              GetIsResized()      const { return isResized; }
        [[nodiscard]] bool              GetIsMinimized()    const { return isMinimized; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetWindow          (HWND& wnd)             { window = wnd; }
        void SetWindowInstance  (HINSTANCE& instance)   { windowInstance = instance; }
        void SetWindowWidth     (int width)             { windowWidth = width; }
        void SetWindowHeight    (int height)            { windowHeight = height; }
        void SetIsResized       (bool resized)          { isResized = resized; }
        void SetIsMinimized     (int minimized)         { isMinimized = minimized; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        /// <summary>
        /// The Render Loop of the Window.
        /// </summary>
        bool Update             ();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        HINSTANCE   windowInstance;
        HWND        window;
        uint32_t    windowWidth;
        uint32_t    windowHeight;

        bool        isResized;
        bool        isMinimized;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static LRESULT CALLBACK WndProc     (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        static void             exitOnError (const wchar_t* msg);
 
    };
    
} // namespace RTR



/*+======================================================================================
File:       Window.cpp

Date:       19th January 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for creating a window.

Functions:  RTR::Window::Window
            RTR::Window::Update

            RTR::Window::WndProc
            RTR::Window::exitOnError
=========================================================================================+*/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "core\Window.h"
// Standard Libraries
#include <cstdio>
#include <cstdlib>
#include <Windowsx.h>

// Macro Definitions

#define APPLICATION_NAME_W  L"RTR"
#define ENGINE_NAME_W       L"Vulkan Engine"

namespace RTR
{
    int Window::MouseXPos        = 0;
    int Window::MouseYPos        = 0;
    int Window::MousePrevXPos    = 0;
    int Window::MousePrevYPos    = 0;

    Window::Window(HINSTANCE hInstance, LPCWSTR appName, int _width, int _height)
    : windowWidth   { static_cast<uint32_t>(_width) }
    , windowHeight  { static_cast<uint32_t>(_height) }
    , isResized     { false }
    , isMinimized   { false }
    {
        WNDCLASSEX wcex;
        wcex.cbSize         = sizeof(WNDCLASSEX);                                       // Size of the WNDCLASSEX struct
        wcex.style          = CS_HREDRAW | CS_VREDRAW;                                           
        wcex.lpfnWndProc    = WndProc;                                                  // Pointer to the window procedure
        wcex.cbClsExtra     = 0;                                                        // Num extra bytes to allocate following the window-class struct
        wcex.cbWndExtra     = 0;                                                        // Num extra bytes to allocate following the window instance
        wcex.hInstance      = hInstance;                                                // Handle to the window instance
        wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);                     // Handle to the class icon (icon resource, ico file etc.)
        wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);                           // Handle to the class cursor
        wcex.hbrBackground  = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));         // Handle to the class background brush
        wcex.lpszMenuName   = nullptr;
        wcex.lpszClassName  = appName;
        wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_WINLOGO);

        if (!RegisterClassEx(&wcex))
        {
            exitOnError(L"Failed to register window!");
        }

        windowInstance = hInstance;
        const uint32_t screenWidth  = GetSystemMetrics(SM_CXSCREEN);
        const uint32_t screenHeight = GetSystemMetrics(SM_CYSCREEN);
        const uint32_t windowX      = (screenWidth >> 1) - (windowWidth >> 1);
        const uint32_t windowY      = (screenHeight >> 1) - (windowHeight >> 1);
        window = CreateWindow
        (  
            APPLICATION_NAME_W, APPLICATION_NAME_W,
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            windowX, windowY, windowWidth, windowHeight, 
            NULL, NULL, windowInstance, NULL
        );

        if (!window)
        {
            exitOnError(L"Failed to create window!");
        }

        if (window)
        {
            SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        }
        

        if (window) ShowWindow(window, SW_SHOW);
        if (window) SetForegroundWindow(window);
        SetFocus(window);

        Utility::LogMessage("Created a Window!");
    }


    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    bool Window::Update()
    {
        MSG message;

        bool shouldQuit = false;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            if (WM_QUIT == message.message)
            {
                shouldQuit = true;
                break;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        return shouldQuit;
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members                                                        */
    /*---------------------------------------------------------------------------------*/

    LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (!window)
            return DefWindowProc(hWnd, message, wParam, lParam);

        switch (message)
        {
            case WM_DESTROY:
            {
                DestroyWindow(hWnd);
                PostQuitMessage(0);
                break; 
            }
            case WM_SIZE:
            {
                if (wParam != SIZE_MINIMIZED)
                {
                    window->SetWindowWidth(static_cast<uint32_t>(LOWORD(lParam)));
                    window->SetWindowHeight(static_cast<uint32_t>(HIWORD(lParam)));
                    window->SetIsResized(true);
                    window->SetIsMinimized(false);
                }
                else
                {
                    window->SetIsMinimized(true);
                }
            }
            case WM_PAINT:
            {
                ValidateRect(hWnd, nullptr);
                break;
            }
            case WM_MOUSEMOVE:
            {
                MousePrevXPos = MouseXPos;
                MousePrevYPos = MouseYPos;
                
                MouseXPos = GET_X_LPARAM(lParam);
                MouseYPos = GET_Y_LPARAM(lParam);
                break;
            }

            default: break;
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void Window::exitOnError(const wchar_t* msg)
    {
        MessageBox(nullptr, msg, ENGINE_NAME_W, MB_ICONERROR);
        exit(EXIT_FAILURE);
    }


} // namespace RTR




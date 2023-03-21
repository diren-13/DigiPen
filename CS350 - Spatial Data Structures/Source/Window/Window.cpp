/************************************************************************************//*!
\file           Window.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Definition of functions declared in Window.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Window/Window.h"
// ImGui Headers
#include "ImGui/imgui_impl_win32.h"
// Project Headers
#include "Tools/Console.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"

/*-------------------------------------------------------------------------------------*/
/* Forward Declarations                                                                */
/*-------------------------------------------------------------------------------------*/
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    int     Window::count       = 0;
    LPCSTR  Window::CLASS_NAME  = "ClamChowder";

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Window::Window(int Width, int Height, const std::string& title)
    : width     { Width }
    , height    { Height }
    , resized   { false }
    , minimised { false }
    {
        // Account for invalid width and height
        if (width <= 0 || height <= 0)
        {
            // Hardcoded to default values
            width   = DEFAULT_WIDTH;
            height  = DEFAULT_HEIGHT;
        }

        // Get handle to current instance
        HINSTANCE currentInstance = GetModuleHandle(nullptr);
        if (count == 0)
        {
            // Register a class for the window 
            WNDCLASSEXA defaultWindowClass{};
            defaultWindowClass.cbSize           = sizeof(WNDCLASSEXA);
            defaultWindowClass.style            = CS_OWNDC;
            defaultWindowClass.lpfnWndProc      = WndProc;
            defaultWindowClass.hInstance        = currentInstance;
            defaultWindowClass.hCursor          = LoadCursor(currentInstance, IDC_ARROW);
            defaultWindowClass.hbrBackground    = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
            defaultWindowClass.lpszClassName    = CLASS_NAME;

            RegisterClassEx(&defaultWindowClass);

            RAWINPUTDEVICE rid{ 0x01, 0x02, 0, nullptr };
            if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
            {
                std::string errorMsg = std::to_string(GetLastError()) + " Failed to register raw input devices.";
                Log(LogSeverity::Error, errorMsg);
            }
        }

        const int SCREEN_WIDTH  = GetSystemMetrics(SM_CXSCREEN);
        const int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
        const int WINDOW_X      = (SCREEN_WIDTH >> 1) - (width >> 1);
        const int WINDOW_Y      = (SCREEN_HEIGHT >> 1) - (height >> 1);

        RECT windowRectangle{ 0 };
        windowRectangle.left    = WINDOW_X;
        windowRectangle.top     = WINDOW_Y;
        windowRectangle.right   = windowRectangle.left + width;
        windowRectangle.bottom  = windowRectangle.top + height;
        AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, false);
        

        handle = CreateWindowEx
        (
            0, CLASS_NAME, title.c_str(), WS_OVERLAPPEDWINDOW, 
            windowRectangle.left, windowRectangle.top, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, 
            nullptr, nullptr, currentInstance, this
        );
        if (!handle)
        {
            Log(LogSeverity::Error, "Error: " + std::to_string(GetLastError()) + " Failed to create a window!");
            return;
        }

        // Show the window, set it as the foreground and the focus window
        ShowWindow(handle, SW_SHOW);
        SetForegroundWindow(handle);
        SetFocus(handle);

#ifdef _DEBUG
        Log(LogSeverity::Info, "Created a window!");
#endif
        ++count;
    }

    Window::~Window()
    {
        if (!DestroyWindow(handle))
        {
            Log(LogSeverity::Error, "Failed to destroy a window!");
        }
        --count;

        if (count == 0)
        {
            UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));
        }
    }


    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    bool Window::Update()
    {
        MSG message;
        ZeroMemory(&message, sizeof(MSG));

        resized     = false;
        minimised   = false;

        while (const BOOL RESULT = PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
                return false;

            if (RESULT < 0)
            {
                Log(LogSeverity::Error, std::to_string(message.message));
                return false;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        return true; 
    }


    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (!hWnd)
        {
            Log(LogSeverity::Error, "Invalid handle passed into WndProc!");
            return 0;
        }

        if (message == WM_CREATE)
        {
            Window* createdWindow = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
            if (!createdWindow)
            {
                Log(LogSeverity::Error, "The created window is a null object!");
                return 0;
            }
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createdWindow));
        }

        if (const auto window = hWnd ? reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) : nullptr; window)
        {
            window->handleMessage(hWnd, message, wParam, lParam);
        }

        if (message == WM_CLOSE)
            return 0;

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT Window::handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;

        switch (message)
        {
            case WM_SIZE:
            {
                if (wParam == SIZE_MINIMIZED)
                {
                    minimised = true;
                    return 0;
                }

                width   = LOWORD(lParam);
                height  = HIWORD(lParam);
                resized = true;
                return 0;
            }
            // Keyboard Messages
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_CHAR:
            case WM_SYSCHAR:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                return handleKeyboard(hWnd, message, wParam, lParam);
            }
            // Mouse Messages
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_INPUT:
            {
                return handleMouse(hWnd, message, wParam, lParam);
            }
            case WM_KILLFOCUS:
            {
                Keyboard::clearState();
                return 0;
            }
            case WM_CLOSE:
            {
                PostQuitMessage(0);
                return 0;
            }
            default: return 0;
        }
    }

    LRESULT Window::handleKeyboard(HWND, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                const unsigned char CHAR = static_cast<unsigned char>(wParam);
                const bool PREV_STATE = lParam & PREV_KEY_STATE_BIT;
                const bool REPEATABLE = Keyboard::GetIsKeyRepeatable();

                if (REPEATABLE || !PREV_STATE)
                {
                    Keyboard::onKeyPressed(CHAR);
                }

                return 0;
            }
            case WM_CHAR:
            case WM_SYSCHAR:
            {
                const unsigned char CHAR = static_cast<unsigned char>(wParam);
                Keyboard::onChar(CHAR);

                return 0;
            }
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                const unsigned char CHAR = static_cast<unsigned char>(wParam);
                Keyboard::onKeyReleased(CHAR);

                return 0;
            }

            default: return 0;
        }
    }

    LRESULT Window::handleMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        const POINTS MOUSE_POS = MAKEPOINTS(lParam);

        switch (message)
        {
            case WM_MOUSEMOVE:
            {
                Mouse::OnMouseMove(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                break;
            }
            case WM_LBUTTONDOWN:
            {
                Mouse::OnLeftPressed(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                break;
            }
            case WM_LBUTTONUP:
            {
                Mouse::OnLeftReleased(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                break;
            }
            case WM_RBUTTONDOWN:
            {
                Mouse::OnRightPressed(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                break;
            }
            case WM_RBUTTONUP:
            {
                Mouse::OnRightReleased(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                break;
            }
            case WM_MBUTTONDOWN:
            {
                Mouse::OnMiddlePressed(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                break;
            }
            case WM_MBUTTONUP:
            {
                Mouse::OnMiddleReleased(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                break;
            }
            case WM_MOUSEWHEEL:
            {
                const auto WHEEL_DIR = GET_WHEEL_DELTA_WPARAM(wParam);
                if (WHEEL_DIR > 0)
                {
                    Mouse::OnWheelUp(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                }

                if (WHEEL_DIR < 0)
                {
                    Mouse::OnWheelDown(MousePosition{ MOUSE_POS.x, MOUSE_POS.y });
                }

                break;
            }
            case WM_INPUT:
            {
                UINT dataSize{};
                GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER));
        
                if (dataSize > 0)
                {
                    std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
                    if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
                    {
                        RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
                        if (raw->header.dwType == RIM_TYPEMOUSE)
                        {
                            Mouse::OnMouseMoveRaw(MousePosition{ raw->data.mouse.lLastX, raw->data.mouse.lLastY });
                        }
                    }
                }

                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            default: break;
        }

        return 0;
    }



}
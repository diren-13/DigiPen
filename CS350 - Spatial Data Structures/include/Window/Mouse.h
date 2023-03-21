/************************************************************************************//*!
\file           Mouse.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of an interface for a Mouse and Mouse Events.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <queue>
// Project Headers
#include "Window/Window.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    struct MousePosition
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        int X = 0;
        int Y = 0;
    };

    class MouseEvent
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        enum class MouseState
        {
            LeftPress
        ,   LeftRelease
        ,   RightPress
        ,   RightRelease
        ,   MiddlePress
        ,   MiddleRelease
        ,   MouseWheelUp
        ,   MouseWheelDown
        ,   Move
        ,   RawMove
        ,   Invalid
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        MouseEvent  ();
        MouseEvent  (MouseState state, MousePosition pos);
        MouseEvent  (MouseState state, int X, int Y);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] MouseState            GetMouseState   ()  const   { return mouseState; }
        [[nodiscard]] const MousePosition&  GetMousePosition()  const   { return position; }

        [[nodiscard]] bool                  IsMouseValid    ()  const   { return mouseState != MouseState::Invalid; }

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        MouseState      mouseState;
        MousePosition   position;
    };

    /********************************************************************************//*!
    @brief    Encapsulates interactions with a Mouse.
    *//*********************************************************************************/
    class Mouse
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] static bool                           GetIsLeftDown   ()  { return leftDown; }
        [[nodiscard]] static bool                           GetIsRightDown  ()  { return rightDown; }
        [[nodiscard]] static bool                           GetIsMiddleDown ()  { return middleDown; }
        [[nodiscard]] static const MousePosition&           GetMousePosition()  { return position; }
        [[nodiscard]] static const std::queue<MouseEvent>&  GetEventBuffer  ()  { return eventBuffer; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static MouseEvent ReadEvent();
  
    private:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static bool                     leftDown;
        static bool                     rightDown;
        static bool                     middleDown;
        static MousePosition            position;

        static std::queue<MouseEvent>   eventBuffer;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static void OnLeftPressed   (const MousePosition& pos);
        static void OnLeftReleased  (const MousePosition& pos);
        static void OnRightPressed  (const MousePosition& pos);
        static void OnRightReleased (const MousePosition& pos);
        static void OnMiddlePressed (const MousePosition& pos);
        static void OnMiddleReleased(const MousePosition& pos);
        static void OnWheelUp       (const MousePosition& pos);
        static void OnWheelDown     (const MousePosition& pos);
        static void OnMouseMove     (const MousePosition& pos);
        static void OnMouseMoveRaw  (const MousePosition& pos);

        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class Window;

    };


}

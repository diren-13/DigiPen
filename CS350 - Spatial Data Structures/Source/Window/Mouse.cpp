/************************************************************************************//*!
\file           Mouse.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Implementation of a Keyboard and Keyboard Event.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include "pch.h"
// Primary Header
#include "Window/Mouse.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    bool                    Mouse::leftDown     = false;
    bool                    Mouse::rightDown    = false;
    bool                    Mouse::middleDown   = false;
    MousePosition           Mouse::position;

    std::queue<MouseEvent>  Mouse::eventBuffer;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    MouseEvent::MouseEvent()
    : mouseState    { MouseState::Invalid }
    , position      {}
    {}

    MouseEvent::MouseEvent(MouseState state, MousePosition pos)
    : mouseState    { state }
    , position      { pos }
    {}

    MouseEvent::MouseEvent(MouseState state, int X, int Y)
    : mouseState    { state }
    , position      { X, Y } 
    {}

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    MouseEvent Mouse::ReadEvent()
    {
        if (eventBuffer.empty())
            return MouseEvent{};

        const MouseEvent EVENT = eventBuffer.front();
        eventBuffer.pop();
        return EVENT;
    }

    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    void Mouse::OnLeftPressed(const MousePosition& pos)
    {
        leftDown = true;
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::LeftPress, pos });
    }

    void Mouse::OnLeftReleased(const MousePosition& pos)
    {
        leftDown = false;
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::LeftRelease, pos });
    }

    void Mouse::OnRightPressed(const MousePosition& pos)
    {
        rightDown = true;
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::RightPress, pos });
    }

    void Mouse::OnRightReleased(const MousePosition& pos)
    {
        rightDown = false;
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::RightRelease, pos });
    }

    void Mouse::OnMiddlePressed(const MousePosition& pos)
    {
        middleDown = true;
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::MiddlePress, pos });
    }

    void Mouse::OnMiddleReleased(const MousePosition& pos)
    {
        middleDown = false;
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::MiddleRelease, pos });
    }

    void Mouse::OnWheelUp(const MousePosition& pos)
    {
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::MouseWheelUp, pos });
    }

    void Mouse::OnWheelDown(const MousePosition& pos)
    {
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::MouseWheelDown, pos });
    }

    void Mouse::OnMouseMove(const MousePosition& pos)
    {
        position = pos;
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::Move, pos });
    }

    void Mouse::OnMouseMoveRaw(const MousePosition& pos)
    {
        eventBuffer.push(MouseEvent{ MouseEvent::MouseState::RawMove, pos });
    }
}
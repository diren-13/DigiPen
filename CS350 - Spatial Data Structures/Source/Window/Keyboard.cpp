/************************************************************************************//*!
\file           Keyboard.cpp
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
#include "Window/Keyboard.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    bool                            Keyboard::repeatKeys    = true;
    std::bitset<Keyboard::NUM_KEYS> Keyboard::keyStates;

    std::queue<KeyboardEvent>   Keyboard::keyBuffer;
    std::queue<unsigned char>   Keyboard::charBuffer;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    KeyboardEvent::KeyboardEvent()
    : keyState  { KeyState::Invalid }
    , keycode   { 0U }
    {}

    KeyboardEvent::KeyboardEvent(KeyState state, unsigned char key)
    : keyState  { state }
    , keycode   { key }
    {}

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    KeyboardEvent Keyboard::ReadKey()
    {
        if (keyBuffer.empty())
            return KeyboardEvent{};

        const KeyboardEvent EVENT = keyBuffer.front();
        keyBuffer.pop();
        return EVENT;
    }

    unsigned char Keyboard::ReadChar()
    {
        if (charBuffer.empty())
            return 0U;

        const unsigned char CHAR = charBuffer.front();
        charBuffer.pop();
        return CHAR;
    }

    bool Keyboard::IsKeyPressed(Keys key)
    {
        return keyStates[key];
    }

    bool Keyboard::IsKeyReleased(Keys key)
    {
        return !IsKeyPressed(key);
    }


    /*---------------------------------------------------------------------------------*/
    /* Private Function Members Definitions                                            */
    /*---------------------------------------------------------------------------------*/

    void Keyboard::clearState()
    {
        keyBuffer = std::queue<KeyboardEvent>();
        charBuffer = std::queue<unsigned char>();
    }

    void Keyboard::onKeyPressed(unsigned char key)
    {
        keyStates[key] = true;
        keyBuffer.push(KeyboardEvent{ KeyboardEvent::KeyState::Pressed, key });
    }

    void Keyboard::onKeyReleased(unsigned char key)
    {
        keyStates[key] = false;
        keyBuffer.push(KeyboardEvent{ KeyboardEvent::KeyState::Released, key });
        trimKeyBuffer(keyBuffer);
    }

    void Keyboard::onChar(unsigned char key)
    {
        charBuffer.push(key);
        trimCharBuffer(charBuffer);
    }

    void Keyboard::trimKeyBuffer(std::queue<KeyboardEvent>& buffer)
    {
        if (buffer.size() > maxBufferSize)
        {
            buffer.pop();
        }
    }

    void Keyboard::trimCharBuffer(std::queue<unsigned char>& buffer)
    {
        if (buffer.size() > maxBufferSize)
        {
            buffer.pop();
        }
    }

}
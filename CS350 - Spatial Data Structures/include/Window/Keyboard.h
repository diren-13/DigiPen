/************************************************************************************//*!
\file           Keyboard.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of an interface for a Keyboard and Keyboard Events.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <queue>
#include <bitset>
// DirectX Headers
#include <Keyboard.h>
// Project Headers
#include "Window/Window.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /********************************************************************************//*!
    @brief    Encapsulates a Keyboard Event.
    *//*********************************************************************************/
    class KeyboardEvent
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief    Represents a Key state.
        *//*****************************************************************************/
        enum class KeyState
        {
            Pressed
        ,   Released
        ,   Invalid
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        KeyboardEvent   ();
        KeyboardEvent   (KeyState state, unsigned char key);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] unsigned char GetKeycode      ()  const   { return keycode; }

        [[nodiscard]] bool          IsKeyPressed    ()  const   { return keyState == KeyState::Pressed; }
        [[nodiscard]] bool          IsKeyReleased   ()  const   { return keyState == KeyState::Released; }
        [[nodiscard]] bool          IsKeyValid      ()  const   { return keyState != KeyState::Invalid; }

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        KeyState        keyState;
        unsigned char   keycode;
    };

    /********************************************************************************//*!
    @brief    Encapsulates interactions with a Keyboard.
    *//*********************************************************************************/
    class Keyboard
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        using Keys = DirectX::Keyboard::Keys;

        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr int NUM_KEYS = 256;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] static bool                               GetIsKeyRepeatable  ()  { return repeatKeys; }
        [[nodiscard]] static const std::queue <KeyboardEvent>&  GetKeyBuffer        ()  { return keyBuffer; }
        [[nodiscard]] static const std::queue <unsigned char>&  GetCharBuffer       ()  { return charBuffer; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        static void    SetIsKeyRepeatable  (bool isKeyRepeatable)  { repeatKeys = isKeyRepeatable; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static KeyboardEvent    ReadKey         ();
        static unsigned char    ReadChar        ();

        static bool             IsKeyPressed    (Keys key);
        static bool             IsKeyReleased   (Keys key);
  
    private:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr unsigned int       maxBufferSize = 16U;

        static bool                         repeatKeys;
        static std::bitset<NUM_KEYS>        keyStates;
         
        static std::queue <KeyboardEvent>   keyBuffer;
        static std::queue <unsigned char>   charBuffer;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static void clearState      ();

        static void onKeyPressed    (unsigned char key);
        static void onKeyReleased   (unsigned char key);
        static void onChar          (unsigned char key);

        static void trimKeyBuffer   (std::queue <KeyboardEvent>& buffer);
        static void trimCharBuffer  (std::queue <unsigned char>& buffer);

        /*-----------------------------------------------------------------------------*/
        /* Friends                                                                     */
        /*-----------------------------------------------------------------------------*/
        friend class Window;
    };
}


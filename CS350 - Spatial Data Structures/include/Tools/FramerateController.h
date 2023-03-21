/************************************************************************************//*!
\file           FramerateController.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 18, 2022
\brief          Contains the interface for the FramerateController.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <chrono>

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    class FrameRateController final
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        enum class Mode
        {
            Uncapped
            , Capped
        };

        /*-----------------------------------------------------------------------------*/
        /* Constants                                                                   */
        /*-----------------------------------------------------------------------------*/
        static constexpr int    DEFAULT_FRAMERATE_TARGET    = 60;
        static constexpr float DEFAULT_FRAMETIME_TARGET     = 1.0f / DEFAULT_FRAMERATE_TARGET;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] static float GetFrameTime() { return lastFrameTime; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static void StartFrame         ();
        static void EndFrame           ();
        static void SetTargetFrameRate (int frameRate = DEFAULT_FRAMERATE_TARGET);
        static void SetTargetFrameTime (float frameTime = DEFAULT_FRAMETIME_TARGET);
        static void SetMode            (Mode mode);

    private:
        static std::chrono::steady_clock::time_point    frameStartTime;
        static float                                    lastFrameTime;
        static float                                    targetFrameTime;
        static Mode                                     currentMode;
    };

}

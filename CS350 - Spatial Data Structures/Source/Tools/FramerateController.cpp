/************************************************************************************//*!
\file           FramerateController.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           June 18, 2022
\brief          Contains the implementation for the FramerateController.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Windows Header
#include <Windows.h>
// Primary Header
#include "Tools/FramerateController.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    std::chrono::steady_clock::time_point   FrameRateController::frameStartTime;
    float                                   FrameRateController::lastFrameTime      = 0.0f;
    float                                   FrameRateController::targetFrameTime    = DEFAULT_FRAMETIME_TARGET;
    FrameRateController::Mode               FrameRateController::currentMode        = FrameRateController::Mode::Uncapped;


    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/
    void FrameRateController::StartFrame()
    {
        // Mark the starting time
        frameStartTime = std::chrono::high_resolution_clock::now();
    }
    void FrameRateController::EndFrame()
    {
        constexpr double MILLISECONDS_PER_SECOND = 1000;

        // Save the frame time
        const std::chrono::duration<float> DELTA = std::chrono::high_resolution_clock::now() - frameStartTime;
        lastFrameTime = DELTA.count();

        // Do frame rate limiting
        if (currentMode == Mode::Capped && lastFrameTime < targetFrameTime)
        {
            const float EXCESS_TIME_SECS   = targetFrameTime - lastFrameTime;
            const int SLEEP_TIME_MILI       = static_cast<int>(EXCESS_TIME_SECS * MILLISECONDS_PER_SECOND);

            lastFrameTime += EXCESS_TIME_SECS;
            Sleep(SLEEP_TIME_MILI);
        }
    }
    void FrameRateController::SetTargetFrameRate(int frameRate)
    {
        SetTargetFrameTime(1.0f / static_cast<float>(frameRate));
    }
    void FrameRateController::SetTargetFrameTime(float frameTime)
    {
        targetFrameTime = std::fabs(frameTime);
    }
    void FrameRateController::SetMode(Mode mode)
    {
        currentMode = mode;
    }

}
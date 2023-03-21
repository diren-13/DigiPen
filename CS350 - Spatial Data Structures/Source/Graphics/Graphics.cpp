/************************************************************************************//*!
\file           Graphics.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Implementation of the lifecycle of the Graphics System.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Graphics.h"
// DirectX Headers
#include <GeometricPrimitive.h>
// Project Headers
#include "Graphics/Engine.h"


namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    std::hash<std::string>  Graphics::StringHash;
    Engine*                 Graphics::engine = nullptr;

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    Engine& Graphics::CreateEngine(Window* window)
    {
        if (engine)
        {
            Log(LogSeverity::Warning, "Engine has already been created!");
            return *engine;
        }

        engine = new Engine{ window };
        return *engine;
    }

    void Graphics::DestroyEngine()
    {
        delete engine;
    }

    //void Graphics::BatchDrawPlane(const Geometry::Plane& plane)
    //{
    //    const std::vector N { std::abs(plane.GetNormal().x), std::abs(plane.GetNormal().y), std::abs(plane.GetNormal().z) };
    //    size_t minIdx = 0;
    //    float minElement = std::numeric_limits<float>::max();
    //    for (size_t i = 0; i < N.size(); ++i)
    //    {
    //        if (N[i] < minElement)
    //        {
    //            minIdx = i;
    //            minElement = N[i];
    //        }
    //    }

    //    CC::Vec3 right{ CC::Vec3::Zero };
    //    switch (minIdx)
    //    {
    //        case 0:
    //        {
    //            right = plane.GetNormal().Cross(CC::Vec3::UnitX);
    //            break;
    //        }
    //        case 1:
    //        {
    //            right = plane.GetNormal().Cross(CC::Vec3::UnitY);
    //            break;
    //        }
    //        case 2:
    //        {
    //            right = plane.GetNormal().Cross(CC::Vec3::UnitZ);
    //            break;
    //        }
    //        default: break;
    //    }

    //    const CC::Vec3 UP = plane.GetNormal().Cross(right);

    //
    //    const CC::Vec3 POP = plane.GetNormal() * plane.GetDistance();
    //    const CC::Vec3 TL = (POP + UP) - (right * 0.5f);
    //    const CC::Vec3 TR = (POP + UP) + (right * 0.5f);
    //    const CC::Vec3 BL = (POP - UP) - (right * 0.5f);
    //    const CC::Vec3 BR = (POP - UP) + (right * 0.5f);

    //    BatchDrawQuad(TL, TR, BR, BL);
    //    BatchDrawLine(POP, POP + plane.GetNormal());

    //}
}

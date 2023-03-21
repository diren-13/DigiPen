/************************************************************************************//*!
\file           Viewport.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Implementation for a DirectX Viewport.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Viewport.h"
// Project Headers
#include "Graphics/Graphics.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Viewport::Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
    : handle { 0 }
    {
        handle.TopLeftX = topLeftX;
        handle.TopLeftY = topLeftY;
        handle.Width    = width;
        handle.Height   = height;
        handle.MinDepth = minDepth;
        handle.MaxDepth = maxDepth;
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    void Viewport::SetTopLeftX(float topLeftX)
    {
        handle.TopLeftX = topLeftX;
    }

    void Viewport::SetTopLeftY(float topLeftY)
    {
        handle.TopLeftY = topLeftY;
    }

    void Viewport::SetWidth(float width)
    {
        handle.Width = width;
    }

    void Viewport::SetHeight(float height)
    {
        handle.Height = height;
    }

    void Viewport::SetMinDepth(float minDepth)
    {
        handle.MinDepth = minDepth;
    }

    void Viewport::SetMaxDepth(float maxDepth)
    {
        handle.MaxDepth = maxDepth;
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Member Definitions                                              */
    /*---------------------------------------------------------------------------------*/

    void Viewport::Set(Device& device) const
    {
        device.GetContext()->RSSetViewports(1U, &handle);
    }


}

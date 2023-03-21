/************************************************************************************//*!
\file           Viewport.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a DirectX Viewport.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// DirectX Headers
#include <d3d11.h>
// Project Headers
#include "Device.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /********************************************************************************//*!
    @brief    Encapsulates a Viewport for rendering to.
    *//*********************************************************************************/
    class Viewport
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Viewport
        (
            float topLeftX  = 0.0f, 
            float topLeftY  = 0.0f, 
            float width     = 1280.0f, 
            float height    = 800.0f,
            float minDepth  = 0.0f,
            float maxDepth  = 1.0f
        );

        /*-----------------------------------------------------------------------------*/
        /* Implicit Conversions                                                        */
        /*-----------------------------------------------------------------------------*/
        operator D3D11_VIEWPORT() const { return handle; }

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const D3D11_VIEWPORT& GetHandle   ()  const   { return handle; }
        [[nodiscard]] float                 GetTopLeftX ()  const   { return handle.TopLeftX; }
        [[nodiscard]] float                 GetTopLeftY ()  const   { return handle.TopLeftY; }
        [[nodiscard]] float                 GetWidth    ()  const   { return handle.Width; }
        [[nodiscard]] float                 GetHeight   ()  const   { return handle.Height; }
        [[nodiscard]] float                 GetMinDepth ()  const   { return handle.MinDepth; }
        [[nodiscard]] float                 GetMaxDepth ()  const   { return handle.MaxDepth; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetTopLeftX (float topLeftX);
        void    SetTopLeftY (float topLeftY);
        void    SetWidth    (float width);
        void    SetHeight   (float height);
        void    SetMinDepth (float minDepth);
        void    SetMaxDepth (float maxDepth);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        void Set(Device& device) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        D3D11_VIEWPORT  handle;
    };
}

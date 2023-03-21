/************************************************************************************//*!
\file           Device.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 20, 2022
\brief          Interface for a DirectX Device.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Windows Headers
#include <wrl/client.h>
// DirectX Headers
#include <d3d11.h>

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Device
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Device  () = default;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Device>&           GetHandle()   { return handle; }
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11DeviceContext>&    GetContext()  { return context; }

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Microsoft::WRL::ComPtr<ID3D11Device>        handle;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    };
}

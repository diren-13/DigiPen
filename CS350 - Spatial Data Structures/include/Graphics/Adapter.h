/************************************************************************************//*!
\file           Adapter.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Encapsulation of the interface for DirectX Adapters.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// STL Headers
#include <vector>
// Windows Headers
#include <wrl/client.h>
// DirectX Headers
#include <d3d11.h>


namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Adapter
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructor                                                                 */
        /*-----------------------------------------------------------------------------*/
        Adapter(IDXGIAdapter* adapter);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] const Microsoft::WRL::ComPtr<IDXGIAdapter>&   GetHandle() const   { return adapter; }
        [[nodiscard]] const DXGI_ADAPTER_DESC&                      GetDesc()   const   { return adapterDesc; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static std::vector<Adapter> GetAdapters();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static std::vector<Adapter> adapters;

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Microsoft::WRL::ComPtr<IDXGIAdapter>    adapter;
        DXGI_ADAPTER_DESC                       adapterDesc;

    };
}

/************************************************************************************//*!
\file           Adapter.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 12, 2022
\brief          Implementation of the interface for DirectX Adapters.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/\

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Graphics/Adapter.h"
// Project Header
#include "Tools/Console.h"

namespace ClamChowder
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    std::vector<Adapter>    Adapter::adapters;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    Adapter::Adapter(IDXGIAdapter* _adapter)
    : adapter       { _adapter }
    {
        const HRESULT HR = adapter->GetDesc(&adapterDesc);
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to initialise an adapter!");
        }
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members Definitions                                             */
    /*---------------------------------------------------------------------------------*/

    std::vector<Adapter> Adapter::GetAdapters()
    {
        if (!adapters.empty())           //If already initialized
            return adapters;

        Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

        // Create a DXGIFactory object.
        const HRESULT HR = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
        if (FAILED(HR))
        {
            Log(LogSeverity::Error, "Failed to create DXGIFactory for enumerating adapters.");
            EXIT_FAILURE;
        }

        IDXGIAdapter * pAdapter;
        UINT index = 0;
        while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
        {
            adapters.emplace_back(Adapter{ pAdapter });
            index += 1;
        }
        return adapters;
    }

}
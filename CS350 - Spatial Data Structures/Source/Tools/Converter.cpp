/************************************************************************************//*!
\file           Converter.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 24, 2022
\brief          Implementation of methods for conversion between types.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Precompiled Header
#include <pch.h>
// Primary Header
#include "Tools/Converter.h"

namespace ClamChowder::Utility
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> StringConverter::converter;

    /*---------------------------------------------------------------------------------*/
    /* Public Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/

    std::wstring StringConverter::StringtoWString(const std::string& str)
    {
        return converter.from_bytes(str);
    }

    std::string StringConverter::WStringtoString(const std::wstring& wStr)
    {
        return converter.to_bytes(wStr);
    }
}
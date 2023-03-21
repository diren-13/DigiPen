/************************************************************************************//*!
\file           Converter.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           May 24, 2022
\brief          Declaration of methods for conversion between types.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// STL Headers
#include <string>
#include <codecvt>
#include <locale>

namespace ClamChowder::Utility
{
    // String Conversions
    class StringConverter
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        static std::wstring    StringtoWString (const std::string& str);
        static std::string     WStringtoString (const std::wstring& wStr);

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    };
    
}

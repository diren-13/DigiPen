/*+======================================================================================
File:       Config.cpp

Date:       2nd February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for a Configuration Instance.
=========================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Headers
#include "Config.h"

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Members                                                             */
    /*---------------------------------------------------------------------------------*/
    Config::Config()
    : enableDebugging   { false }
    , enableRenderDoc   { false }
    {}
}
/*+======================================================================================
File:       Utilities.cpp

Author:     Diren D Bharwani

Summary:    Implementation for Utility functions.

Classes:    RTR::Utility::LogMessage
            RTR::Utility::LogHeader
            RTR::Utility::LogWarning
            RTR::Utility::LogError
            RTR::Utility::LogException
======================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Standard Libraries
#include <iomanip>
// Primary Header
#include "Utilities.h"


namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/
    void Utility::LogMessage(const char* message)
    {
        std::cout << "\x1b[97m " << message << std::endl;
    }

    void Utility::LogHeader(const char* message)
    {
        std::cout   << "\n--------------------------------------------------\n "
                    << message << "\n"
                    << "--------------------------------------------------\n";
    }


    void Utility::LogWarning(const char* message)
    {
        std::cerr << "\x1b[93m " << message << "\x1b[97m" << std::endl;
    }

    void Utility::LogError(const char* message)
    {
        std::cerr << "\x1b[91m " << message << "\x1b[97m" << std::endl;
    }

    void Utility::LogException(const char* message)
    {
        std::cerr << "\x1b[101m " << message << "\x1b[49m" << std::endl;
    }
} // namespace RTR
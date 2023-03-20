/*+======================================================================================
File:       Utilities.h

Date:       13th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for the Utility class containing Utility functions.

Classes:    RTR::Utility
======================================================================================+*/

#pragma once

// Standard Libraries
#include <string>

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    class Utility
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        /// <summary>
        /// Logs a message onto the console.
        /// </summary>
        /// <param name="message"> The message to log. </param>
        static void LogMessage(const char* message);
        /// <summary>
        /// Logs a header onto the console.
        /// </summary>
        /// <param name="message">The message for the header</param>
        static void LogHeader(const char* message);
        /// <summary>
        /// Logs a warning message onto the console.
        /// </summary>
        /// <param name="message"> The message to log. </param>
        static void LogWarning(const char* message);
        /// <summary>
        /// Logs a non-critical error message onto the console.
        /// </summary>
        /// <param name="message"> The message to log. </param>
        static void LogError(const char* message);
        /// <summary>
        /// Logs an exception message onto the console.
        /// </summary>
        /// <param name="message"> The message to log. </param>
        static void LogException(const char* message);

    private:
        Utility()  = delete;
        ~Utility() = delete;
    };


} // namespace RTR

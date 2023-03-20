/*+======================================================================================
File:       Config.h

Date:       2nd February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Interface for creating a Configuration Instance.

Classes:    RTR::Config
=========================================================================================+*/

#pragma once

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a configuration registry for the Application
    /// </summary>
    struct Config
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Config();

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        bool    enableDebugging;
        bool    enableRenderDoc;
    };
    

} // namespace RTR
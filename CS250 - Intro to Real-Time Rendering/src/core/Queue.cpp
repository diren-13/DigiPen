/*+======================================================================================
File:       Queue.cpp

Date:       8th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Implementation for the Queue class.

Functions:  RTR::Queue::Queue

            RTR::Queue::IsValid
=========================================================================================+*/

// Precompiled Headers
#include "pch.h"
// Primary Header
#include "core\Queue.h"

namespace RTR
{
    Queue::Queue()
    : familyIndex   {-1}
    , index         {-1}
    {}

    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    bool Queue::IsValid()
    {
        return familyIndex > -1 && index > -1;
    }

} // namespace RTR
/*+======================================================================================
File:       Queue.h

Date:       8th February 2022

Author:     Diren D Bharwani
            diren.dbharwani@digipen.edu

Summary:    Defines an interface for interacting with Queues in Vulkan.

Classes:    RTR::Queue
=========================================================================================+*/

#pragma once

// Standard Libraries
#include <cstdint>

namespace RTR
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /// <summary>
    /// Encapsulates a Vulkan Queue.
    /// </summary>
    struct Queue
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors                                                                */
        /*-----------------------------------------------------------------------------*/
        Queue();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline int32_t   GetFamilyIndex()    const { return familyIndex; }
        [[nodiscard]] inline int32_t   GetIndex()          const { return index; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] inline int32_t&  FamilyIndex()   { return familyIndex; }
        [[nodiscard]] inline int32_t&  Index()         { return index; }

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/

        /// <summary>
        /// Checks if the Queue is valid.
        /// </summary>
        /// <returns>
        /// True if the queue is valid.
        /// </returns>
        bool IsValid();

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        int32_t familyIndex;
        int32_t index;
    };

} // namespace RTR

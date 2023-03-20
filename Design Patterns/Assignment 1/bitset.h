/************************************************************************************//*!
\file           bitseh.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Sep 14, 2021
\brief          Contains the interface for a clone of the C++ standard library 
                bitset class.
 
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef BITSET_H
#define BITSET_H

// Standard libraries
#include <cstdlib>   // size_t
#include <string>

namespace csd2125
{
    template <size_t N>
    class bitset
    {
      public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructors                                                  */
        /*-----------------------------------------------------------------------------*/
        /****************************************************************************//*!
        @brief	    Default constructor for bitset.
        *//*****************************************************************************/
        constexpr bitset();

        /****************************************************************************//*!
        @brief	    Destructor for bitset.
        *//*****************************************************************************/
        ~bitset();

        /*-----------------------------------------------------------------------------*/
        /* Operator Overloads			     						   				   */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief	    Overloads the subscript operator.

        @param[in]  pos
            The position of the bit to retrieve
        
        @returns    A reference to the bit.
        *//*****************************************************************************/
        bool operator [](size_t pos) const;

        /*-----------------------------------------------------------------------------*/
        /* Member Functions				     						   				   */
        /*-----------------------------------------------------------------------------*/

        /****************************************************************************//*!
        @brief	    Sets the value for the bit at the indicated position.

        @param[in]  pos
            The position of the bit to set.
        @param[in]  value
            The value of the bit to set; either true(1) or false(0). Defaults to true(1).
        *//*****************************************************************************/
        void set(size_t pos, bool value = true);

        /****************************************************************************//*!
        @brief	    Clears the bit at the position indicated

        @param[in]  pos
            The position of the bit to reset.
        *//*****************************************************************************/
        void reset(size_t pos);

        /****************************************************************************//*!
        @brief	    Flips the bit at the position indicated

        @param[in]  pos
            The position of the bit to flip.
        *//*****************************************************************************/
        void flip(size_t pos);

        /****************************************************************************//*!
        @brief	    Converts the bitset to a string.

        @param[in]  clearBit
            The char to replace cleared bits with in the string.
        @param[in]  setBit
            The char to replace cleared bits with in the string.

        @returns    The bitset as a string.
        *//*****************************************************************************/
        std::string to_string(char clearBit = '0', char setBit = '1') const;

        /****************************************************************************//*!
        @brief	    Counts the number of set bits.

        @returns    The number of set bits.
        *//*****************************************************************************/
        size_t count() const;

        /****************************************************************************//*!
        @brief	    Getter for the size of the bitset

        @returns    The size of the bitset.
        *//*****************************************************************************/
        size_t size() const;

        /****************************************************************************//*!
        @brief	    Checks if the bit at the position has been set.

        @param[in]  pos
            The position of the bit to set.
        
        @returns    True if the bit is set to 1. False if set to 0.
        *//*****************************************************************************/
        bool test(size_t pos) const;

      private:
        /*-----------------------------------------------------------------------------*/
        /* Member Variables				     						   				   */
        /*-----------------------------------------------------------------------------*/
        uint8_t* bits;
        
    };
} // namespace csd2125

#endif // BITSET_H

// Template definitions
#include "bitset.hpp"

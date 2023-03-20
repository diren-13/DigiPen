/************************************************************************************//*!
\file           bitseh.hpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Sep 14, 2021
\brief          Contains the implementation for a clone of the C++ standard library 
                bitset class.
 
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef BITSET_HPP
#define BITSET_HPP

// Primary Header
#include "bitset.h"
// Standard Libraries
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <climits>

namespace csd2125
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructors                                                      */
    /*---------------------------------------------------------------------------------*/

    template <size_t N>
    constexpr bitset<N>::bitset()
    {
        if (N <= 0) 
        {
            throw std::invalid_argument("Size of bitset cannot be lesser or equal to 0!");
        }

        bits = new uint8_t[(N / CHAR_BIT) + (N % CHAR_BIT ? 1 : 0)];
        for (size_t i = 0; i < N; ++i) { reset(i); }
    } 

    template <size_t N>
    bitset<N>::~bitset()
    {
        delete[](bits);
    }

    /*---------------------------------------------------------------------------------*/
    /* Operator Overloads                                                              */
    /*---------------------------------------------------------------------------------*/

    template <size_t N>
    bool bitset<N>::operator [](size_t pos) const
    {
        if (pos >= N || pos < 0) { throw std::out_of_range("Bit position is out of range!"); }

        size_t addressOffset = pos / CHAR_BIT;
        size_t bitOffset = pos % CHAR_BIT;

        return *(bits + addressOffset) >> bitOffset & 1U;
    }

    /*---------------------------------------------------------------------------------*/
    /* Member Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    template <size_t N>
    void bitset<N>::set(size_t pos, bool value)
    {
        // Check if input pos is valid
        if (pos >= N || pos < 0) { throw std::out_of_range("Bit position is out of range!"); }

        // Get appropriate offsets
        size_t addressOffset = pos / CHAR_BIT;
        size_t bitOffset = pos % CHAR_BIT;

        // Set bit
        value ? *(bits + addressOffset) |= static_cast<uint8_t>(1U << bitOffset) :
                *(bits + addressOffset) &= static_cast<uint8_t>(~(1U << bitOffset)) ;      
    }

    template <size_t N>
    void bitset<N>::reset(size_t pos)
    {
        if (pos >= N || pos < 0) { throw std::out_of_range("Bit position is out of range!"); }

        size_t addressOffset = pos / CHAR_BIT;
        size_t bitOffset = pos % CHAR_BIT;

        *(bits + addressOffset) &= static_cast<uint8_t>(~(1U << bitOffset));
    }

    template <size_t N>
    void bitset<N>::flip(size_t pos)
    {
        if (pos >= N || pos < 0) { throw std::out_of_range("Bit position is out of range!"); }

        size_t addressOffset = pos / CHAR_BIT;
        size_t bitOffset = pos % CHAR_BIT;

        *(bits + addressOffset) ^= static_cast<uint8_t>(1U << bitOffset);
    }

    template <size_t N>
    std::string bitset<N>::to_string(char clearBit, char setBit) const
    {
        std::stringstream ss;
        for (size_t i = 1; i <= N; ++i)
        {
            char bitValue = (*this)[N-i] ? setBit : clearBit;
            ss << bitValue;
        }
        return ss.str();
    }

    template <size_t N>
    size_t bitset<N>::count() const
    {
        size_t out = 0;
        for (size_t i = 0; i < N; ++i)
        {
            if ((*this)[i]) { ++out; }
        }
        return out;
    }

    template <size_t N>
    size_t bitset<N>::size() const
    {
        return N;
    }

    template <size_t N>
    bool bitset<N>::test(size_t pos) const
    {
        return (*this)[pos];
    }

} // namespace csd2125

#endif // BITSET_HPP
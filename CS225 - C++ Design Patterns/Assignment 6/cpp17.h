/************************************************************************************//*!
\file           cpp17.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Nov 25, 2021
\brief          Implementation of template meta-programming to generate a reverse
                sequence of 2^N -> 2^0

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

#include "index_sequence.h"

/************************************************************************************//*!
\brief      Recursively generates a reverse sequence of numbers from 2^First to 2^0.

\tparam     Counter
    A counter to keep track of how many instances have been made.
\tparam		First
	The original value passed in.
\tparams	Values
	A variadic set of unsigned long values to instantiate the class with.
*//*************************************************************************************/
template <uint64_t Counter, uint64_t First, uint64_t... Values>
auto power_of_2_rindex_sequence_impl()
{
    if constexpr (Counter == 0)
    {
        return index_sequence<1UL << First, Values...>{};
    }
    else
    {
        return power_of_2_rindex_sequence_impl<Counter - 1, First, 1UL << (First - Counter), Values...>();
    }
}

template<uint64_t _N>
using make_sequence = decltype(power_of_2_rindex_sequence_impl<_N, _N>());
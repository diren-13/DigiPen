#ifndef INDEX_SEQUENCE_H
#define INDEX_SEQUENCE_H

#include <iostream>

template <size_t... Ns>
struct index_sequence 
{
	static void print() 
	{
		#ifdef CPP11
		std::cout << "[C++11]\t";
		#else
		std::cout << "[C++17]\t";
		#endif
		
		size_t const numbers[] = {Ns...};
		for (auto const& number : numbers) 
		{
			std::cout << number << ", ";
		}
		std::cout << std::endl;
	}
};

#endif // INDEX_SEQUENCE_H

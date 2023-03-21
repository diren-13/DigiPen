#include <iostream>		// std::cout
#include "splitter.h"	// splitter-joiner interface

int main(int argc, char *argv[]) 
{
    CSD2125::SplitResult rs = CSD2125::split_join(argc, argv);
    
    switch(rs) 
    {
        case CSD2125::SplitResult::E_BAD_SOURCE:
        {
            std::cout << "Unable to open or read from input file\n";
            break;
        }
        case CSD2125::SplitResult::E_BAD_DESTINATION:
        {
            std::cout << "Unable to open or write to output file\n";
            break;
        }
        case CSD2125::SplitResult::E_NO_MEMORY:
        {
            std::cout << "Unable to allocate heap memory\n";
            break;
        }
        case CSD2125::SplitResult::E_SMALL_SIZE:
        {
            std::cout << "Negative or zero buffer size\n";
            break;
        }
        case CSD2125::SplitResult::E_NO_ACTION:
        {
            std::cout << "No action specified by user\n";
            break;
        }
        case CSD2125::SplitResult::E_SPLIT_SUCCESS:
        {
            std::cout << "Split successfully completed\n";
            break;
        }
        case CSD2125::SplitResult::E_JOIN_SUCCESS:
        {
            std::cout << "Join successfully completed\n";
            break;
        }
    }
}

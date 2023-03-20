#ifndef SPLITTER_HPP
#define SPLITTER_HPP

#include <string>
#include <vector>


namespace CSD2125 
{

    // don't change this macro/constant because if you allocate
    // more memory than this, the online grader crash your submission
    // since it has been set up with some pre-defined limits
    // that will prevent your program from accessing more than this
    // limit. Just don't do it!!!
    // Until C++17, it was not possible to define a single instance of
    // objects such as MAX_SPLIT_SIZE for the entire program. Now, it is
    // possible to do that using the inline specifier. 
    inline constexpr int MAX_SPLIT_SIZE{4096};

    /****************************************************************************//*!
    @brief  A result of the split join operation.
    *//*****************************************************************************/
    enum class SplitResult 
    {
        E_BAD_SOURCE,           // Input file for split or join doesnt exist
        E_BAD_DESTINATION,      // Output file for split or join cannot be created
        E_NO_MEMORY,            // std::bad_alloc
        E_SMALL_SIZE,           // Byte size for split <= 0
        E_NO_ACTION,            // Number of CL args < 2 or -j/-s switch is missing
        E_SPLIT_SUCCESS,        // File has been sucessfully split
        E_JOIN_SUCCESS          // Files have been sucessfully joined
    };

    /****************************************************************************//*!
    @brief  Takes in a set of command line arguments and joins or splits a
            binary file depending on the switches provided
                    
    @param[in]  argc
        The number of command line arguments
    @param[in]  argv
        The command line arguments

    @return Returns the result depending on the state of the execution.
    *//*****************************************************************************/
    SplitResult split_join(int argc, char *argv[]);

    
}

#endif 

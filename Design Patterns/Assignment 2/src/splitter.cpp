/************************************************************************************//*!
\file           CollideCircle.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Sep 28, 2021
\brief          Contains the implementation of the Circle Collision Detection algorithms.
 
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "splitter.h"
// Standard Libraries
#include <iostream>
#include <iomanip>
#include <fstream> 
#include <sstream>
#include <iterator>     
#include <algorithm>    // std::copy_n
#include <type_traits>  // std::is_integral
     

namespace CSD2125
{
    /********************************************************************************//*!
    @brief  Encapsulates the state of the split_join.
    *//*********************************************************************************/
    enum class State
    {
        SPLIT,
        JOIN
    };

    /********************************************************************************//*!
    @brief  Splits a binary file into multiple files.
                    
    @param[in]  inputFilePath
        The input file path.
    @param[in]  outputFilePrefix
        The prefix for the output files.
    @param[in]  chunkSize
        The chunk size.

    @return Returns the result depending on the state of the execution.
    *//*********************************************************************************/
    SplitResult split(const std::string& inputFilePath, const std::string& outputFilePrefix, int chunkSize);

    /********************************************************************************//*!
    @brief  Joins multiple binary files into one file.
                    
    @param[in]  inputFiles
        The container of input files
    @param[in]  outputFileoutputFilePath
        The file path to put the joined files in.

    @return Returns the result depending on the state of the execution.
    *//*********************************************************************************/
    SplitResult join(const std::vector<std::string>& inputFiles, const std::string& outputFilePath);

    /****************************************************************************//*!
    @brief  Takes in a set of command line arguments and joins or splits a
            binary file depending on the switches provided
                    
    @param[in]  argc
        The number of command line arguments
    @param[in]  argv
        The command line arguments

    @return Returns the result depending on the state of the execution.
    *//*****************************************************************************/
    SplitResult split_join(int argc, char* argv[])
    {
        SplitResult result = SplitResult::E_NO_ACTION;
        State state = State::SPLIT;

        // Insufficient arguments passed in
        if (argc < 2) { return result; }
        // Skip argv[0] as it is the executable

        // Variables to use
        // For split
        int splitChunkSize = 0;         // store the chunk size if files are being split
        std::string outputFilePrefix;

        // For join
        std::string outputFile;
        std::vector<std::string> inputFiles;
        
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i][0] == '-')
            {
                switch (argv[i][1])
                {
                    case 's':   // S switch
                    {
                        state = State::SPLIT;        
                        // get the chunk size
                        ++i;
                        splitChunkSize = std::stoi(argv[i]);
                        break;
                    }
                    case 'j':
                    {
                        state = State::JOIN;
                        break;
                    }
                    case 'o':   // Output arg
                    {
                        // Get the output file
                        ++i;
                        state == State::SPLIT ? outputFilePrefix = argv[i] : outputFile = argv[i] ;
                        break;
                    }
                    case 'i':   // Input arg
                    {
                        ++i;
                        if (state == State::SPLIT)
                        {
                            // Only 1 input file for splitting      
                            inputFiles.emplace_back(argv[i]);
                        }
                        else 
                        {
                            // Loop through all possible args 
                            for (int j = i; j < argc; ++j)
                            {
                                if (argv[j][0] == '-') { break; }   // Exit loop when a dash is found if not end of args
                                // Get all input files
                                inputFiles.emplace_back(argv[j]);
                            }
                        }
                        break;
                    }
                }
            }
        }

        if (state == State::SPLIT)
        {
            result = split(inputFiles.front(), outputFilePrefix, splitChunkSize); 
        }
        else
        {
            result = join(inputFiles, outputFile);
        }

        return result;
    }

    SplitResult split(const std::string& inputFilePath, const std::string& outputFilePrefix, int chunkSize)
    {
        /*
        Args are as follows in this order for split
        idx     desc
        ---     ----
        0       chunk size
        1       output switch   
        2       output file path prefix
        3       input switch    
        4       input file path
        */

        // Perform split
        int fileIdx = 1;
        std::ifstream inputFile;

        inputFile.open(inputFilePath, std::ios::binary);
        // Error handling
        if (!inputFile.is_open()) { return SplitResult::E_BAD_SOURCE; }

        inputFile.seekg (0, inputFile.end);
        int remainingLength = static_cast<int>(inputFile.tellg());
        inputFile.seekg (0, inputFile.beg);

        // Determine number of files;
        while (remainingLength > 0)
        {
            // Pad file number with 0's
            std::stringstream outputFilePath;
            outputFilePath << outputFilePrefix << std::setfill('0') << std::setw(4) << fileIdx;

            std::ofstream outputFile;
            outputFile.open(outputFilePath.str(), std::ios::binary);
            // Error handling
            if (!outputFile.is_open()) { return SplitResult::E_BAD_DESTINATION; }

            // Allocate space for buffer
            const int splitSize = std::min(remainingLength, std::min(chunkSize, MAX_SPLIT_SIZE));
            char* buffer;
            try
            {
                buffer = new char[splitSize];
            }
            catch(const std::bad_alloc& e)
            {
                return SplitResult::E_NO_MEMORY;
            }
            
            // Copy over elements
            inputFile.read(buffer, splitSize);
            outputFile.write(buffer, splitSize);

            // Clean up
            delete[] buffer;

            // reduce chunk size        
            remainingLength -= splitSize; 
            ++fileIdx;
        }

        return SplitResult::E_SPLIT_SUCCESS;
    }

    SplitResult join(const std::vector<std::string>& inputFiles, const std::string& outputFilePath)
    {
        std::ofstream outputFile;
        outputFile.open(outputFilePath, std::ios::binary);
        // Error handling
        if (!outputFile.is_open()) { return SplitResult::E_BAD_DESTINATION; }

        // int outputFilePos = 0;
        for (auto& filePath : inputFiles)
        {
            std::ifstream inputFile;
            inputFile.open(filePath, std::ios::binary);
            // Error handling
            if (!inputFile.is_open()) { return SplitResult::E_BAD_SOURCE; }

            inputFile.seekg (0, inputFile.end);
            int inputFileLength = static_cast<int>(inputFile.tellg());
            inputFile.seekg (0, inputFile.beg);

            std::copy_n(std::istreambuf_iterator<char>(inputFile), inputFileLength, std::ostreambuf_iterator<char>(outputFile));
        }

        return SplitResult::E_JOIN_SUCCESS;
    }
}

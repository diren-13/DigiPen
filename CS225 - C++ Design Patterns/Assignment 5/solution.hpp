/************************************************************************************//*!
\file           solution.hpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Nov 18, 2021
\brief          Contains the implementation of the following functions:

 
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

/************************************************************************************//*!
\brief      Prints all the file names of a file stored in a map of file records.

\param      map
    Reference to an associative container holding all the file records.
*//*************************************************************************************/
void print_file_names(const file_records& map)
{
    std::for_each(std::cbegin(map), std::cend(map), std::bind(print_file_name, std::bind(split, std::placeholders::_1)));
}

/************************************************************************************//*!
\brief      Prints all the file names of non-empty files stored in a map of file records.

\param      map
    Reference to an associative container holding all the file records.

\returns    The number of non-empty files
*//*************************************************************************************/
size_t print_non_empty_files(const file_records& map)
{
    // get all non-empty files
    std::vector<file_record> nonEmptyFiles;
    std::copy_if(std::cbegin(map), std::cend(map), std::back_inserter(nonEmptyFiles), 
                 std::bind(check_if_empty, std::placeholders::_1, true));
    
    // print all non-empty file names
    std::for_each(std::cbegin(nonEmptyFiles), std::cend(nonEmptyFiles), std::bind(print_file_name, std::bind(split, std::placeholders::_1)));
    return std::size(nonEmptyFiles);
}

/************************************************************************************//*!
\brief      Prints all the file names of empty files stored in a map of file records.

\param      map
    Reference to an associative container holding all the file records.

\returns    The number of empty files
*//*************************************************************************************/
size_t print_empty_files(const file_records& map)
{
    // Get all empty files
    std::vector<file_record> emptyFiles;
    std::copy_if(std::cbegin(map), std::cend(map), std::back_inserter(emptyFiles), 
                 std::bind(check_if_empty, std::placeholders::_1, false));
    
    // print all empty file names
    std::for_each(std::cbegin(emptyFiles), std::cend(emptyFiles), std::bind(print_file_name, std::bind(split, std::placeholders::_1)));
    return std::size(emptyFiles);
}

/************************************************************************************//*!
\brief      Gets the parameters in an associative container as a tuple.

\param      map
    Reference to an associative container holding all the file records.
*//*************************************************************************************/
std::tuple<file_records&> get_parameters(file_records& map)
{
    return std::forward_as_tuple(map);
}

/************************************************************************************//*!
\brief      Removes all empty files from an associative container.

\param      map
    Reference to an associative container holding all the file records.
*//*************************************************************************************/
void remove_empty(file_records& map)
{
    // Get all non-empty files
    std::vector<file_record> nonEmptyFiles;
    std::copy_if(std::begin(map), std::end(map), std::back_inserter(nonEmptyFiles), 
                 std::bind(check_if_empty, std::placeholders::_1, true));

    // clear the map
    std::function<void(file_records&)> mapClear = &file_records::clear;
    mapClear(map);

    // Put non-empty files into map
    std::for_each(std::begin(nonEmptyFiles), std::end(nonEmptyFiles), std::bind(insert, std::ref<file_records>(map), std::placeholders::_1));
}
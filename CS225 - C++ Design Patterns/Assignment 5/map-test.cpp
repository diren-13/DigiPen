#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <utility>

using file_name = std::string;
using file_size = size_t;
using file_record = std::pair<file_name, file_size>;
using file_records = std::map<file_name, file_size>;

void insert(file_records& records, file_record const& record) 
{
	records.insert(record);
}

bool check_if_empty(file_record const& record, bool trueIfIsNot) 
{
	file_size size;
	std::tie(std::ignore, size) = record;
	bool result = (size == 0);
  	result = trueIfIsNot ? !result : result;
	return result;
}

file_name split(file_record const& record) 
{
	file_name name;
	std::tie(name, std::ignore) = record;
	return name;
}

void print_file_name(file_name const& name) 
{
	std::cout << " * " << name << std::endl;
}

/*******************************************************************************
 * Inside solution.h you need to define the following functions:
 *
 * - print_file_names(const file_records& map);
 * - print_non_empty_files(const file_records& map);
 * - print_empty_files(const file_records& map);
 * - get_parameters(file_records& map);
 * - remove_empty(file_records& map);
 *
 * Refer to the documentation for the constraints that you must observe.
 ******************************************************************************/

#include "solution.hpp"

////////////////////////////////////////////////////////////////////////////////

int main() 
{
	file_records map = 
    {
		{"readme.txt", 1000},
		{"main.exe", 12000},
		{"save.bak", 0},
		{"library.dll", 1234},
		{"0.res", 120000},
		{"1.res", 120000},
		{"2.res", 115900},
		{"errors.log", 0}
	};

	std::cout << "Files:" << std::endl;
	print_file_names(map);
	std::cout << std::endl;

	std::cout << "Files that are not empty:" << std::endl;
	const size_t nonEmptyCount = print_non_empty_files(map);
	std::cout
		<< " There are "
		<< nonEmptyCount
		<< " non-empty files.\n"
		<< std::endl;

	std::cout << "Files that are empty:" << std::endl;
	const size_t emptyCount = print_empty_files(map);
	std::cout
		<< " There are "
		<< emptyCount
		<< " empty files.\n"
		<< std::endl;

	std::cout << "Files after removing the empty ones:" << std::endl;
	auto parameters = get_parameters(map);
	std::apply(remove_empty, parameters);
	print_file_names(map);
}

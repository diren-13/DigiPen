#include <iostream>
#include <vector>
#include <memory>
#include <array>
#include "allocator.hpp"

// unit tests ...
// functions declared using trailing return type [see page 229 of text] ...
auto test1() -> void;
auto test2() -> void;
auto test3() -> void;
auto test4() -> void;

// driver ...
int main() 
{
	using Test = void (*)();
    std::array<Test, 4> tests {test1, test2, test3, test4};

	int i = 0;
	for (Test const& test : tests) 
    {
		try 
        {
			std::cout << (++i) << ". ";
			test();
			std::cout << "\n";
		} catch (std::exception const& e) 
        {
			std::cout << "\nError: " << e.what() << "\n";
		} catch (...) 
        {
			std::cout << "\nUnknown error has occurred." << "\n";
		}
	}
}

auto test1() -> void 
{
	std::cout << "Global new/delete:\n---------" << "\n";

	std::vector<csd2125::vertex> vector;
	vector.push_back(csd2125::vertex{});
	vector.push_back(csd2125::vertex{0.0f, 1.0f, 2.0f, 3.0f});
}

auto test2() -> void 
{
	std::cout << "In-class new/delete:\n---------" << "\n";

	csd2125::vector* vp = new csd2125::vector(10.0f, 20.0f, 30.0f, 40.0f);
	std::cout << "  (" << vp->x << ", " << vp->y << ", " << vp->z << ", " << vp->w << ")" << "\n";
	delete vp;
}

auto test3() -> void 
{
	std::cout << "Placement new with explicit delete:\n---------" << "\n";

	csd2125::vertex v1{0.0f, 1.0f, 2.0f, 3.0f};
	float data[sizeof(csd2125::vertex) / sizeof(float)];

	// Placement new in the pre-allocated memory block "data".
	csd2125::vertex* vp2 = new (data) csd2125::vertex{v1};
	
	std::cout
		<< "  ("
		<< vp2->vertexCoordinates.x << ", "
		<< vp2->vertexCoordinates.y << ", "
		<< vp2->vertexCoordinates.z << ", "
		<< vp2->vertexCoordinates.w
		<< ") - object as a structure" << "\n";
	std::cout
		<< "  ("
		<< vp2->axisCoordinates[0] << ", "
		<< vp2->axisCoordinates[1] << ", "
		<< vp2->axisCoordinates[2] << ", "
		<< vp2->axisCoordinates[3]
		<< ") - object as an array" << "\n";
	std::cout
		<< "  ("
		<< data[0] << ", "
		<< data[1] << ", "
		<< data[2] << ", "
		<< data[3]
		<< ") - memory as an array" << "\n";
		
	// Explicit destructor call; the memory will not be released until
	// its variable with automatic storage will reach the end of the scope.
	vp2->~vertex();
}

auto test4() -> void 
{
	std::cout << "Allocator:\n---------" << "\n";

	csd2125::allocator<csd2125::vertex, short> allocator;
	using pointer = decltype(allocator)::pointer;
	pointer pv1 = allocator.allocate(10);
	pointer pv2 = allocator.allocate(4);
	allocator.deallocate(pv1, 10);
    pointer pv3 = allocator.allocate(16);
	pointer pv4 = allocator.allocate(8);

	allocator.deallocate(pv4, 8);
	allocator.deallocate(pv3, 16);
	allocator.deallocate(pv2, 4);
	pointer pv5 = allocator.allocate(32);
	allocator.deallocate(pv5, 32);
	std::cout << "\n";
}
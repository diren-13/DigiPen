# name of C++ compiler
CXX       = g++
# options to C++ compiler
CXX_FLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic-errors  -g
# flag to linker to make it link with math library
LDLIBS    = -lm
# list of object files
OBJS      = allocator.o
# name of executable program
EXEC      = allocator.out

# full compile 
all : $(EXEC)
.PHONY : all
$(EXEC) : $(OBJS)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $(EXEC) $(LDLIBS)

# target allocator.o depends on both allocator-test.cpp and allocator.hpp
allocator.o : allocator-test.cpp allocator.hpp
	$(CXX) $(CXX_FLAGS) -c allocator-test.cpp -o allocator.o
	
# removes all generated files
.PHONY : clean
clean :
	rm -f $(OBJS) $(EXEC)

# removes the generated files and recompiles and runs
.PHONY : rebuild
rebuild :
	$(MAKE) clean
	$(MAKE)

# to run tests
.PHONY : test
test : $(EXEC)
	./$(EXEC) > test-output.txt
	diff -y --suppress-common-lines test-output.txt allocator-test.txt
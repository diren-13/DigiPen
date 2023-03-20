# name of C++ compiler
CXX       = g++
# options to C++ compiler
CXX_FLAGS = -std=c++17 -Wall -Wextra -Werror -Wconversion -pedantic-errors  -g
# flag to linker to make it link with math library
LDLIBS    = -lm
# list of object files
OBJS      = matrix.o
# name of executable program
EXEC      = matrix.out

# full compile 
all : $(EXEC)
.PHONY : all
$(EXEC) : $(OBJS)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $(EXEC) $(LDLIBS)

# target matrix.o depends on both matrix-test.cpp and matrix.hpp
matrix.o : matrix-test.cpp matrix.hpp
	$(CXX) $(CXX_FLAGS) -c matrix-test.cpp -o matrix.o
	
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
	diff -y --strip-trailing-cr --suppress-common-lines test-output.txt matrix-test.txt
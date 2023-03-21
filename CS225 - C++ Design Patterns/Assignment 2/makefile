# Macros ========================================

CC=g++
VG=valgrind
CFLAGS= -std=c++17 -Wall -Wextra -Werror -pedantic-errors -Wconversion 
CPP=src/driver.cpp src/splitter.cpp
EXE=main
ERASE=rm
CHUNK_SIZE=19
SPLIT_OUTPUT=split-data/test1_
SPLIT_INPUT=data/test1
JOIN_OUTPUT=joined-data/new_test1
JOIN_INPUT=$(SPLIT_OUTPUT)*

# Targets ========================================

$(EXE) : $(CPP) makefile.gnu
	$(CC) $(CFLAGS) $(CPP) -o $(EXE) 
	./$(EXE) -s $(CHUNK_SIZE) -o $(SPLIT_OUTPUT) -i $(SPLIT_INPUT)
	./$(EXE) -j -o $(JOIN_OUTPUT) -i $(JOIN_INPUT)
	diff $(SPLIT_INPUT) $(JOIN_OUTPUT)
#	$(VG) ./$(EXE)
#	doxygen Doxyfile

clean :
	$(ERASE) $(EXE)

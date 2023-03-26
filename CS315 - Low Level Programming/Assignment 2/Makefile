CC = g++
CFLAGS = -O2

edge: edge.cpp bmp.cpp bmp.h measure.h kirsch.h 
	g++ -std=c++17 -O2 -mavx -mavx2 -msse4 main.cpp edge.cpp bmp.cpp -o vpl_execution

edge2: edge2.cpp bmp.cpp bmp.h measure.h kirsch.h 
	g++ -std=c++17 -O2 -mavx -mavx2 -msse4 main.cpp edge2.cpp bmp.cpp -o vpl_execution


edg2e.o: edge.cpp bmp.h
bmp.o: bmp.cpp bmp.h
main.o: main.cpp kirsch.h bmp.h measure.
clean:
	rm -f *.o *~
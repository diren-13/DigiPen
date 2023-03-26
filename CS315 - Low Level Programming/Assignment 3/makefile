CC = g++

poly_opt: 
	g++ -std=c++17 -O2 -Wall -Werror -pedantic-errors opt_poly.hpp main.cpp opt_poly.cpp -o vpl_execution

poly_opt_asm:
	gcc -std=c17 -O2 -m64 -Wall -Werror -pedantic-errors opt_poly.c -S -o poly_opt.asm

clean:
	rm -f *.o *~

main.o: main.cpp measure.hpp opt_poly.hpp opt_poly.cpp
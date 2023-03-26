#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "clock.h"

typedef void (*test_func) (int *, int);

const int TIMES = 10;

#define MAX_DIM 1000

typedef enum {
    BASIC,
    OPT,
    ORIGINAL,
    END
} ARRAY_STATE;

void init_array(int *G, int dim)
{

    for (int i = 0; i < dim; ++i)

        for (int j = 0; j < dim; ++j)

            G[i * dim + j] = rand() % 2;

}

bool check_correctness(int *G, int *H, int dim)
{

    for (int i = 0; i < dim; ++i)

        for (int j = 0; j < dim; ++j)

            if (G[i * dim + j] != H[i * dim + j]) {

                return false;

            }

    return true;

}

void opt_col_convert_single_threaded(int *G, int dim)
{
    /*  Fill in your code here */
    int i, j;

    for (i = 0; i < dim; ++i)

        for (j = 0; j < dim; ++j) {

            G[j * dim + i] = G[j * dim + i] || G[i * dim + j];

        }
}

void basic_col_convert(int *G, int dim)
{

    int i, j;

    for (i = 0; i < dim; ++i)

        for (j = 0; j < dim; ++j) {

            G[j * dim + i] = G[j * dim + i] || G[i * dim + j];

        }

}

int main(int argc, char **argv)
{

    prof_time_t start, end;

    prof_time_t base_time;

    test_func tf_array[] = {

        basic_col_convert,
        opt_col_convert_single_threaded,
    };

    const char *output_strings[] = {
        "Basic",
        "Optimal Single Threaded",
    };

    std::cerr << sizeof(unsigned) << std::endl;

    std::cerr << get_clock_speed() << std::endl;

    start_read(start);

    end_read(end);

    base_time = end - start;

    int *arrays[END];

    arrays[BASIC] = new int[MAX_DIM * MAX_DIM];

    arrays[OPT] = new int[MAX_DIM * MAX_DIM];

    arrays[ORIGINAL] = new int[MAX_DIM * MAX_DIM];

    init_array(arrays[ORIGINAL], MAX_DIM);


    /*
       Goes through every version from BASIC to OPT_MULTI

       Right now, every implementation is the same as the basic.
     */

    for (int which_func = BASIC; which_func < ORIGINAL; ++which_func) {

        std::cout << output_strings[which_func] << std::endl;

        for (int i = 10; i <= MAX_DIM; i += 30) {

            prof_time_t acc_t;

            acc_t = 0;

            memcpy(arrays[which_func], arrays[ORIGINAL],
                   sizeof(int) * MAX_DIM * MAX_DIM);

            do {

                start_read(start);

                tf_array[which_func] (arrays[which_func], i);

                end_read(end);

                acc_t = end - start - base_time;
            }
            while (!find_convergence(acc_t, 300, 0.01));

            if (which_func != BASIC) {

                memcpy(arrays[BASIC], arrays[ORIGINAL],
                       sizeof(int) * MAX_DIM * MAX_DIM);

                tf_array[BASIC] (arrays[BASIC], i);

                if (!check_correctness
                    (arrays[BASIC], arrays[which_func], i)) {

                    std::cerr << "Incorrect!" << std::endl;
                    return -1;
                }

            }


            /*
               Prints out the number of elements and cycles taken.
             */
            std::cout << i * i << "\t" << acc_t << std::endl;

        }

    }

    return 0;

}

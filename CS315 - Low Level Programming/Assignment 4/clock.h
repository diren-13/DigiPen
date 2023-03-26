#ifndef CLOCK_H
#define CLOCK_H
#include <cstdlib>
#ifdef __linux
#include "scheduler.h"
#include "unistd.h"
typedef unsigned long prof_time_t;
#elif defined(_WIN32)
#include <Windows.h>
#include "intrin.h"
typedef unsigned long long prof_time_t;
#endif

inline void start_read(prof_time_t & start)
{
    unsigned *low = (unsigned *) &start;
    unsigned *high = low + 1;
    int a[4], b = 0;
    __cpuid(a, b);
    start = __rdtscp((unsigned int *) &a[0]);
};

void inline end_read(prof_time_t & end)
{
    unsigned *low = (unsigned *) &end;
    unsigned *high = low + 1;
    int a[4];
    unsigned int x;
    end = __rdtscp(&x);
    __cpuid(a, (int) x);
};


double inline get_clock_speed()
{
    prof_time_t start, end;

    start_read(start);
#ifdef __linux
    sleep(1);
#elif defined(_WIN32)
    Sleep(1000);
#endif
    end_read(end);
    return (double) (end - start);
}

inline bool find_convergence(prof_time_t & val, int num_of_samples,
                             double epsilon)
{
    static prof_time_t smallest = 0xFFFFFFFFFFFFFFFFL;
    static int num_times = 0;

    if (num_times >= num_of_samples) {
        num_times = 0;
        val = smallest;
        smallest = 0xFFFFFFFFFFFFFFFFL;
        return true;
    }

    num_times++;

    if (llabs(val - smallest) >= (epsilon * val)) {
        if (val < smallest)
            smallest = val;
        return false;
    } else {
        val = smallest;
        {
            smallest = 0xFFFFFFFFFFFFFFFFL;
            num_times = 0;
            return true;
        }
    }
}

#endif

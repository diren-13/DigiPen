#ifndef CLOCK_H
#define CLOCK_H
#ifdef _WIN32
#include <Windows.h>
#include <intrin.h>
typedef unsigned __int64 prof_time_t;
#elif defined(__linux)
typedef unsigned long long prof_time_t;
#endif

inline void start_measure(prof_time_t & cycle_read)
{
#ifdef _WIN32
    SetProcessAffinityMask(GetCurrentProcess(), 1);
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    int a[4], b = 1;
    __cpuid(a, b);
    cycle_read = __rdtsc();
#else
    unsigned &high = *(((unsigned *) &cycle_read + 1));
    unsigned &low = *(((unsigned *) &cycle_read));
    asm volatile ("cpuid\n\t"
                  "rdtsc\n\t"
                  "movl %%eax, %0\n\t"
                  "movl %%edx, %1\n\t":"=r" (low), "=r"(high)
                  ::"%eax", "%ebx", "%ecx", "%edx");
#endif
}

void inline end_measure(prof_time_t & cycle_read)
{
#ifdef _WIN32
    unsigned int c;
    cycle_read = __rdtscp(&c);
    int a[4], b = 1;
    __cpuid(a, b);
#else
    int &high = *(((int *) &cycle_read + 1));
    int &low = *(((int *) &cycle_read));
    asm volatile ("rdtscp\n\t"
                  "movl %%edx, %0\n\t"
                  "movl %%eax, %1\n\t" "cpuid\n\t":"=r" (high), "=r"(low)
                  ::"%eax", "%ebx", "%ecx", "%edx");
#endif
}

#endif

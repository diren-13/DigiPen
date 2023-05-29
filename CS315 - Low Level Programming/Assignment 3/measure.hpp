/****************************************************************************************
 * \file      measure.h
 * \author    Diren D Bharwani, diren.dbharwani, 390002520
 * \brief     Interface for functions to measure the number of cycles operations take.
 *
 * \copyright Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 *            disclosure of this file or its contents without the prior written consent
 *            of DigiPen Institute of Technology is prohibited.
****************************************************************************************/

#ifndef MEASURE_H
#define MEASURE_H 

#ifdef _WIN32

  #include <Windows.h>
  #include <intrin.h>
  
  #define sleep(s) Sleep(1000*s)

  typedef unsigned __int64 cyc_time_t;

#elif defined(__linux)

  typedef unsigned long long cyc_time_t;

#endif

#include <chrono>
#include <thread>

class PerfClock
{
public:

  static double MeasureFunction(void(*func)(), unsigned int numReps)
  {
    cyc_time_t start  = 0;
    cyc_time_t end    = 0;

    // Get base time
    start = measure();
    end   = measure();
    cyc_time_t base = end - start;

    start = measure();

    for (unsigned int i = 0; i < numReps; ++i)
      func();

    end   = measure();

    cyc_time_t cycles = end - start - base;
    return (cycles / static_cast<double>(numReps));
  }

public:

  static inline cyc_time_t measure()
  {
    unsigned int lo, hi;
    asm volatile  
    (
      "lfence \n\t"
      "rdtsc \n\t"
      : "=a"(lo), "=d"(hi)
    );

    cyc_time_t cycles = ((cyc_time_t)hi << 32) | lo;
    return cycles;
  }

  static cyc_time_t inline getNumCyclesPerSecond()
  {
    const cyc_time_t START  = measure();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const cyc_time_t END    = measure();

    return END - START;
  }

};

#endif
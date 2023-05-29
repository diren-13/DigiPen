
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "measure.hpp"
#include "opt_poly.hpp"

#define MEASURE_COUNT   100000
#define NUM_ELEMS_MULT  50
#define UPPER_RAND      9
#define LOWER_RAND      1

int main()
{
  Math::RNG::init();

  double(*funcs[3])(double*, double, long) = { Math::poly, Math::polyh, Math::poly_opt };
  double cycles[3] = { 0.0 };

  // Open file to write to
  std::ofstream csvFile { "cpe2.csv" };
  if (!csvFile.is_open())
  {
    std::cout << "Failed to open file! Exiting program..." << std::endl;
    return 0;
  }

  // Headers
  csvFile << "Num Elems,poly,polyh,opt_poly" << std::endl;
  // First line
  csvFile << "0,0,0,0" << std::endl;

  long numSteps = 40L;
  for (long step = 1L; step <= numSteps; ++step)
  {
    const long    NUM_ELEMS = step * NUM_ELEMS_MULT;
    const double  x         = static_cast<double>(Math::RNG::generateNumber<int>(1, 4));

    std::vector<double> coeffs;
    for (long i = 0L; i < NUM_ELEMS; ++i)
      coeffs.emplace_back(static_cast<double>(Math::RNG::generateNumber<int>(LOWER_RAND, UPPER_RAND)));

    // Get base time
    cyc_time_t start  = 0;
    cyc_time_t end    = 0;

    start = PerfClock::measure();
    end   = PerfClock::measure();
    cyc_time_t base = end - start;

    for (int i = 0; i < 3; ++i)
    {
      start = PerfClock::measure();
      {
        for (unsigned int j = 0; j < MEASURE_COUNT; ++j)
          funcs[i](coeffs.data(), x, NUM_ELEMS);
      }
      end   = PerfClock::measure();

      cyc_time_t numCycles = end - start - base;
      cycles[i] = (numCycles / static_cast<double>(MEASURE_COUNT));
    }

    csvFile << std::fixed << std::setprecision(4) 
            << NUM_ELEMS  << ","
            << cycles[0]  << ","
            << cycles[1]  << ","
            << cycles[2]  << std::endl;
  }

  csvFile.close();

  return 0;
}
/* Start Header *****************************************************************/ 

/*! \file cpu.cpp

    \author Diren D Bharwani, diren.dbharwani, 390002520 

    \par    diren.dbharwani@digipen.edu

    \date   Sept 13, 2022 

    \brief  Copyright (C) 2022 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.

*/ 

/* End Header *******************************************************************/

#include <cstring>
#include "heat.h"

////////////////////////////////////////////////////////////////////

extern "C" void initPoints
(
  float*  pointIn,
  float*  pointOut,
  uint    nRowPoints
)
{
  // Initialise all internal points to 0
  // Edge points are 26.67 & 65.56 [(0,10),(0,30)]
  // Edge points are row == 0, col == 0, row == nRowPoints - 1, col == nRowPoints -1
  static constexpr float EDGE_VALUE = 26.67f;
  static constexpr float SPECIAL_EDGE_VALUE = 65.56f;

  for (uint row = 0; row < nRowPoints; ++row)
  {
    for (uint col = 0; col < nRowPoints; ++col)
    {
      const bool IS_EDGE = (row == 0) || (col == 0) || (row == nRowPoints - 1) || (col == nRowPoints - 1);
      const uint IDX = row * nRowPoints + col;

      if (!IS_EDGE)
      {
        pointIn[IDX] = pointOut[IDX] = 0.0f;
      }
      else
      {
        const bool SPECIAL_EDGE = (row == 0 && (col >= 10 && col <= 30));
        pointIn[IDX] = pointOut[IDX] = (SPECIAL_EDGE ? SPECIAL_EDGE_VALUE : EDGE_VALUE);
      }
    }
  }
}

extern "C" void heatDistrCPU
(
  float*  pointIn,
  float*  pointOut,
  uint    nRowPoints,
  uint    nIter
)
{
  const size_t NUM_BYTES = static_cast<size_t>(nRowPoints * nRowPoints) * sizeof(float);

  while (nIter > 0)
  {
    for (uint row = 0; row < nRowPoints; ++row)
    {
      for (uint col = 0; col < nRowPoints; ++col)
      {
        const bool IS_EDGE = (row == 0) || (col == 0) || (row == nRowPoints - 1) || (col == nRowPoints - 1);
        if (IS_EDGE)
          continue;

        const uint T_IDX = (row + 1) * nRowPoints + col;
        const uint B_IDX = (row - 1) * nRowPoints + col;
        const uint R_IDX = row * nRowPoints + (col + 1);
        const uint L_IDX = row * nRowPoints + (col - 1);

        const float SUM = pointIn[T_IDX] + pointIn[B_IDX] + pointIn[R_IDX] + pointIn[L_IDX];
        pointOut[row * nRowPoints + col] = SUM * 0.25f;
      }
    }

    memcpy(pointIn, pointOut, NUM_BYTES);
    --nIter;
  }
}

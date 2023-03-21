/* Start Header *****************************************************************/ 

/*! \file kernel.cu

    \author Diren D Bharwani, diren.dbharwani, 390002520 

    \par    diren.dbharwani@digipen.edu

    \date   Sept 13, 2022 

    \brief  Copyright (C) 2022 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.

*/ 

/* End Header *******************************************************************/

#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <helper_cuda.h>
#include <helper_functions.h>

////////////////////////////////////////////////////////////////////

#define BLOCK_SIZE 32
typedef unsigned int uint;

__global__
void heatDistrCalc(float* in, float* out, uint nRowPoints)
{
  const uint ROW = blockDim.y * blockIdx.y + threadIdx.y;
  const uint COL = blockDim.x * blockIdx.x + threadIdx.x;

  if (ROW < nRowPoints && COL < nRowPoints)
  {
    const uint IDX = ROW * nRowPoints + COL;

    const bool IS_EDGE = (ROW == 0) || (COL == 0) || (ROW == nRowPoints - 1) || (COL == nRowPoints - 1);
    if (IS_EDGE)
    {
      out[IDX] = in[IDX];
    }
    else
    {
      const uint T_IDX = (ROW + 1) * nRowPoints + COL;
      const uint B_IDX = (ROW - 1) * nRowPoints + COL;
      const uint R_IDX = ROW * nRowPoints + (COL + 1);
      const uint L_IDX = ROW * nRowPoints + (COL - 1);

      const float SUM = in[T_IDX] + in[B_IDX] + in[R_IDX] + in[L_IDX];
      out[ROW * nRowPoints + COL] = SUM * 0.25f;
    }
  }
}

///not required in A1
///Shared memory kernel function for heat distribution calculation
__global__
void heatDistrCalcShm(float* in, float* out, uint nRowPoints)
{

}

__global__
void heatDistrUpdate(float* in, float* out, uint nRowPoints)
{
  const uint ROW = blockDim.y * blockIdx.y + threadIdx.y;
  const uint COL = blockDim.x * blockIdx.x + threadIdx.x;

  if (ROW < nRowPoints && COL < nRowPoints)
  {
    const uint IDX = ROW * nRowPoints + COL;
    out[IDX] = in[IDX];
  }
}

extern "C" void heatDistrGPU
(
  float*  d_DataIn,
  float*  d_DataOut,
  uint    nRowPoints,
  uint    nIter
)
{
  dim3 DimBlock(BLOCK_SIZE, BLOCK_SIZE, 1);
  dim3 DimGrid2(ceil(((float)nRowPoints) / BLOCK_SIZE), ceil(((float)nRowPoints) / BLOCK_SIZE), 1);

  for (uint k = 0; k < nIter; k++) 
  {
    heatDistrCalc<<<DimGrid2, DimBlock>>>((float*)d_DataIn, (float*)d_DataOut, nRowPoints);
    getLastCudaError("heatDistrCalc failed\n");
    cudaDeviceSynchronize();

    heatDistrUpdate<<<DimGrid2, DimBlock>>>((float*)d_DataOut, (float*)d_DataIn, nRowPoints);
    getLastCudaError("heatDistrUpdate failed\n");
    cudaDeviceSynchronize();
  }
}

/* Start Header *****************************************************************/ 

/*! \file   kernel.cu

    \author Diren D Bharwani, diren.dbharwani, 390002520 

    \par    diren.dbharwani@digipen.edu

    \date   Oct 2, 2022 

    \brief  Copyright (C) 2022 DigiPen Institute of Technology.
            Reproduction or disclosure of this file or its contents without the
            prior written consent of DigiPen Institute of Technology is prohibited.

FUCK CUDA
*/ 

/* End Header *******************************************************************/

#include <cuda_runtime_api.h>
#include <device_launch_parameters.h>
#include <helper_cuda.h>
#include <helper_functions.h>
#include "helper.h"

//////////////////////////////////////////////////////////////////////////////////

//P and M column-major, N row-major
__global__ void matrixMultiply
(
  FLOAT_TYPE*       P   //<! [out]  an mxn matrix
, const FLOAT_TYPE* A   //<! [in]   an mxk matrix
, const FLOAT_TYPE* B   //<! [in]   an kxn matrix
, const int         m
, const int         n
, const int         k
) 
{
  // Shared memory for tiling input N array
  __shared__ FLOAT_TYPE B_s[TILE_WIDTH_RATIO_K][TILE_WIDTH_N];

  // Output array
  FLOAT_TYPE pReg[TILE_WIDTH_N] { 0.0f };

  for (int nIter = 0; nIter < (k - 1) / TILE_WIDTH_RATIO_K + 1; ++nIter)
  {
    // Load data into shared memory
    const unsigned int B_ROW = nIter * TILE_WIDTH_RATIO_K + threadIdx.x / TILE_WIDTH_N;
    const unsigned int B_COL = blockIdx.y * TILE_WIDTH_N + threadIdx.x % TILE_WIDTH_N;
    const unsigned int B_IDX = B_ROW * n + B_COL;

    const unsigned int SHARED_ROW = threadIdx.x / TILE_WIDTH_N;
    const unsigned int SHARED_COL = threadIdx.x % TILE_WIDTH_N;

    const bool WITHIN_B_BOUNDARY = static_cast<int>(B_ROW) < k && static_cast<int>(B_COL) < n;
    B_s[SHARED_ROW][SHARED_COL] = WITHIN_B_BOUNDARY ? B[B_IDX] : 0.0f;

    __syncthreads();

    // Calculate Partial Sums
    for (int i = 0; i < TILE_WIDTH_RATIO_K; ++i)
    {
      const unsigned int A_ROW = blockIdx.x * TILE_WIDTH_M + threadIdx.x;
      const unsigned int A_COL = i + nIter * TILE_WIDTH_RATIO_K;
      const unsigned int A_IDX = A_COL * m + A_ROW;

      const bool WITHIN_A_BOUNDARY = static_cast<int>(A_ROW) < m && static_cast<int>(A_COL) < k;
      const FLOAT_TYPE M_REG = WITHIN_A_BOUNDARY ? A[A_IDX] : 0.0f;

      for (int j = 0; j < TILE_WIDTH_N; ++j)
        pReg[j] += M_REG * B_s[i][j];
    }

    __syncthreads();
  }

  for (int i = 0; i < TILE_WIDTH_N; ++i)
  {
    const unsigned int OUTPUT_ROW = blockIdx.x * TILE_WIDTH_M + threadIdx.x;
    const unsigned int OUTPUT_COL = blockIdx.y * TILE_WIDTH_N + i;
    //const unsigned int OUTPUT_IDX = OUTPUT_ROW * n + OUTPUT_COL;
    const unsigned int OUTPUT_IDX = OUTPUT_COL * m + OUTPUT_ROW;

    const bool WITHIN_OUTPUT_BOUNDARY = static_cast<int>(OUTPUT_ROW) < m && static_cast<int>(OUTPUT_COL) < n;
    atomicAdd(&P[OUTPUT_IDX], WITHIN_OUTPUT_BOUNDARY ? pReg[i] : static_cast<FLOAT_TYPE>(0));
  }
}

void matrixMultiplyGPU(FLOAT_TYPE* C,
  FLOAT_TYPE* A,
  FLOAT_TYPE* B,
  int numARows,
  int numBColumns,
  int numAColumns)
{
  //@@ Initialize the grid and block dimensions here

  dim3 dimGrid((numARows - 1) / TILE_WIDTH_M + 1, (numBColumns - 1) / TILE_WIDTH_N + 1);
  dim3 dimBlock(TILE_WIDTH_M, 1);

  matrixMultiply << <dimGrid, dimBlock >> > (C,
    A,
    B,
    numARows,
    numBColumns,
    numAColumns);

  getLastCudaError("matrixMultiply failed\n");
  cudaDeviceSynchronize();
}

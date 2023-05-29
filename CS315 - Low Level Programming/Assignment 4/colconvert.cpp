/****************************************************************************************
 * \file      colconvert.cpp
 * \author    Diren D Bharwani, diren.dbharwani, 390002520
 * \brief     Implementation for a cache-friendly function that converts a directed graph
 *            into an undirected graph.
 *
 * \copyright Copyright (C) 2022 DigiPen Institute of Technology. Reproduction or
 *            disclosure of this file or its contents without the prior written consent
 *            of DigiPen Institute of Technology is prohibited.
****************************************************************************************/

#define BLOCK 8
#define MIN(lhs, rhs) lhs < rhs ? lhs : rhs

/**
 * @brief
 * An optimised conversion of an adjacency matrix directed graph to an
 * undirected graph. The optimisation is done through accessing blocks
 * of values that are spatially and locally 
 * @param graph
 * Pointer to the array of nodes.
 * @param dim
 * The number of rows & columns in the array of nodes.
 */
void opt_col_convert_single_threaded(int* graph, int dim)
{
  // Outer loop iterates per block
  for (int i = 0; i < dim; i += BLOCK)
  {
    // Clamp the block to prevent overshoot.
    const int MAX_BLOCK_IDX = MIN(i + BLOCK, dim);

    // Per element double-for loop
    for (int j = i; j < dim; ++j)
      for (int k = i; k < MAX_BLOCK_IDX; ++k)
      {
        /*
          We store the value of both symmetric cells and set the value for both
          This way, we save on iterations as well.

          We also have already loaded both values, so the cache is hot.
          The overall loop follows an inverted-L pattern.
          This allows us to access values that have already been loaded into the cache
          that are not conflicting with each other.

          By using a smaller block size, we abuse spatial and temporal locality.
          In the 1st iteration, we access. (0,0),(1,4),(2,12),(3,16). We miss 0, 4, 12 & 16.
          In the 2nd iteration, we have loaded all 4 rows, so all subsequent accesses will be hits.
          We will only override the cache much later when we reach an index that conflicts with the currently loaded rows.

          | 0 | 1 | 2 | 3 |
          | 4 | 5 | 6 | 7 | 
          | 12| 13| 14| 15|
          | 16| 17| 18| 19|
        */

        const int A = graph[k * dim + j];
        const int B = graph[j * dim + k];

        const int RESULT = A || B;

        // The order of assignment matters. 
        // Since we last accessed [j * dim + k], we access it immediately after again.
        graph[j * dim + k] = RESULT;
        graph[k * dim + j] = RESULT;
      }
  }
}
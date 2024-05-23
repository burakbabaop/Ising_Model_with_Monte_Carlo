/*\
 *
 * This module implements utility functions to access an n-dim grid of bits
 *
\*/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ranlux.h"
#include "grid.h"


// Calculate the nth power of x (all integer)
int ipow(
      int x, /* Base */
      int n  /* Exponent */) {
   int prod = 1;
   for (int i = 0; i < n; i++) { prod *= x; }
   return prod;
}

// Allocate the memory for a grid on the heap
// returns -1 if malloc failed
int alloc_grid(
   int d,      /* Number of dimensions */
   int n,      /* Number of points in each dimension */
   char** grid /* Pointer to the grid pointer*/) {

   int v = ipow(n, d);
   *grid = (char*)malloc((v - 1) / 8 + 1);
   if (*grid == NULL) { return -1; }
   memset(*grid, 0x00,  (v - 1) / 8 + 1);
   return 0;
}

// Initialise a grid with all gridpoints set to 1
void initialise_cold(
   int d,      /* Number of dimensions */
   int n,      /* Number of points in each dimensions */
   char* grid  /* Grid pointer */) {

   int v = ipow(n, d);
   memset(grid, 0xff,  (v - 1) / 8 + 1);
}

// Initialise a grid with a prandom distribution of 1's and 0's
void initialise_hot(
   int d,      /* Number of dimensions */
   int n,      /* Number of points in each dimensions */
   char* grid  /* Grid pointer */) {

   // Generate as many prandom numbers as there are bytes in the grid,
   // scale them up to 255 to have a uniformly distributed prandom number in [0, 255]
   // floor the value to get a uniformly distributed integer prandom number.
   // In uniformly distributed integer numbers each bit is uniformly distributed over
   // [0, 1] and therefor has a 50/50 change to be 0 or 1
   // Proof: By contraposition, assume one bit would have a change p >= 0.5, than finding
   // numbers with that bit set are higher, that will lead to spikes in the distribution
   // at each number with that bit set.
   int bytes = (ipow(n, d) - 1 ) / 8 + 1;
   double* prndm_nmbrs = (double*)malloc(bytes * sizeof(double));
   char* prndm_bits= (char*)malloc(bytes);
   ranlxd(prndm_nmbrs, bytes);
   for (int i = 0; i < bytes; i++) { prndm_nmbrs[i] *= 256; prndm_bits[i] = floor(prndm_nmbrs[i]); }
   memcpy(grid, prndm_bits, bytes);
   free(prndm_nmbrs);
   free(prndm_bits);
}

int getn(
   char* grid,    /* Grid pointer */
   int d,         /* Number of dimension */
   int n,         /* Number of points in each dimension */
   int index      /* Index of the point to get */) {

   int linearised_index = index / 8;
   return grid[linearised_index] >> (index % 8) & 0x01;
}

// Set a gridpoint to 1
void raisen(
   char* grid,    /* Grid pointer */
   int d,         /* Number of dimension */
   int n,         /* Number of points in each dimension */
   int index      /* Index of the point to get */) {

   int linearised_index = index / 8;
   grid[linearised_index] |= (0x01 << index % 8);
}

// Set a gridpoint to 0
void lowern(
   char* grid,    /* Grid pointer */
   int d,         /* Number of dimension */
   int n,         /* Number of points in each dimension */
   int index      /* Index of the point to get */) {

   int linearised_index = index / 8;
   grid[linearised_index] &= ~(0x01 << index % 8);
}

// Toggle a gridpoint
void togglen(
   char* grid,    /* Grid pointer */
   int d,         /* Number of dimension */
   int n,         /* Number of points in each dimension */
   int index      /* Index of the point to get */) {

   int linearised_index = index / 8;
   grid[linearised_index] ^= (0x01 << index % 8);
}

// Set a gridpoint to value
// Note: Every even value will set the gridpoint to 0 while
//       every odd  value will set the gridpoint to 1
void setn(
   char* grid,    /* Grid pointer */
   int d,         /* Number of dimension */
   int n,         /* Number of points in each dimension */
   int index,     /* Index of the point to get */
   char value     /* The value to set (either 0 or 1) */) {

   int linearised_index = index / 8;
   grid[linearised_index] &= ~(0x01 << index % 8);
   grid[linearised_index] |= ((value & 0x01) << index % 8);
}

// calculate coordinates from index
void inv_idx(int i,      /* index*/
             int d,      /* number of dimensions*/
             int n,      /* number of points in each direction*/
             int* coords /* coordinate pointer*/) {
	for (int j = d - 1; j >= 0; j--) {
		coords[j] = i / ipow(n, j);
		i -= coords[j] * ipow(n, j);
	}
}

// calculate index from coordinates
int idx(int d,      /* number of dimensions*/
        int n,      /* number of points in each direction*/
        int* coords /* coordinate pointer*/) {
   int result = 0;
   int k = 1;
   for (int i = 0; i < d; i++) {
      result += coords[i] * k;
      k *= n;
   }
   return result;
}

// Utility to print the grid
void print_grid(char* grid, /* grid pointer*/
                int d,      /* number of dimensions*/
                int n       /* number of points in each direction*/) {
   int coords[2];
   for (int y = 0; y < n; y++)
   {
   	for (int x = 0; x < n; x++)
   	{
   		coords[0] = x;
   		coords[1] = y;
   		printf("%d ", getn(grid, 2, 9, idx(d, n, coords)));
   	}
   	printf("\n");
   }
   return;
}

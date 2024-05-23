/*
 * Initialises a random 4x4 grid
*/

#include <stdio.h>
#include "grid.h"
#include "ranlux.h"

int main() {
	rlxd_init(2, 12345);
	char* grid;
	int coords[2];
	int N = 4;
	int D = 2; 
	alloc_grid(D, N, &grid);
	initialise_hot(D, N, grid);
	print_grid(grid, D, N);
}

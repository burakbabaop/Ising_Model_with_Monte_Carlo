/*
 * Stores 2 replicas containing 10 grids of a 9x1 grid in the imdf file format
 */

#include <stdio.h>
#include "grid.h"
#include "imdf.h"


int main() {
    int N = 9;
    int D = 1;
    int replicas = 2;
    int replica_length = 10;
    struct imdf frame;
	char* grid;

    imdf_allocate(&frame, N, D, replicas, replica_length);
    alloc_grid(D, N, &grid);
    grid[0] = 0xff;
    for (int j=0; j < replicas; j++) {
        for (int i=0; i < replica_length; i++) {
            imdf_store_grid(&frame, j, i, grid);
        }
    }
    imdf_write_to_file(&frame, "test.imd");
	return 0;
}

/*
 * Benchmarks the metropolis i.e. measures the runtime in seconds per iteration
 * as a function of gridsize
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"
#include "metropolis.h"
#include "ranlux.h"

int main(int argc, char** argv)
{
    rlxd_init(2, 42069);
    int D = 2;
    int NMAX = 1024;
    int V;
    int iterations = 128;
    char* grid;
    int* magnetisation;
    double* energy;
    magnetisation = (int*)malloc(iterations * sizeof(int));
    energy = (double*)malloc(iterations * sizeof(double));
    if (magnetisation == NULL || energy == NULL) { return -2; }
    for (int N = 2; N <= NMAX; N*=2)
    {
        if (alloc_grid(D, N, &grid) != 0) { printf("Fatal error"); return -1; }
        V = ipow(N,D);
        clock_t time1;
        clock_t time2;
        time1 = clock();
        metro(grid, D, N, 1.0, 1.0, iterations, magnetisation, energy);
        time2 = clock();
        printf("%d\t%f\n", N, ((double)(time2 - time1)) / (CLOCKS_PER_SEC *  iterations));
        free(grid);
    }
    free(energy);
    free(magnetisation);
}

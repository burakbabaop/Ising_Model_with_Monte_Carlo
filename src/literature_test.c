#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "grid.h"
#include "ranlux.h"
#include "metropolis.h"

int main(int argc, char** argv) {
    rlxd_init(2, 12345);
    int D = 2;
    int N = 100;
    double beta = 0.43;
    double b = 0.01;
    int iterations = 1000;
    char* grid;
    int* magnetisation;
    double* energies;
    if (alloc_grid(D, N, &grid) != 0) { printf("Fatal error"); return -1; }
    if (!(magnetisation = (int*)malloc(iterations * sizeof(int)))) { printf("Fatal error"); return -2; }
    if (!(energies = (double*)malloc(iterations * sizeof(double)))) { printf("Fatal error"); return -3; }

    metro(grid, D, N, beta, b, iterations, magnetisation, energies);

    // Now take the last 100 entries in magnetisation and energies to calculate the mean value
    // and compare that to the literature value given by Prof. Patella
    double mean_energy = 0;
    double mean_magnetisation = 0;
    int sample_size = 500;
    for (int i = iterations - sample_size; i < iterations; i++) {
        mean_energy += energies[i] / sample_size;
        mean_magnetisation += magnetisation[i] / sample_size;
    }
    printf("rel. energy error: %f\n", (mean_energy + 6236.0) / 6236.0);
    printf("rel. energy error: %f\n", (mean_magnetisation - 7182.0) / 7182.0);
}

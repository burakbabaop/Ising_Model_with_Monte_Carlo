#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "grid.h"
#include "ranlux.h"
#include "metropolis.h"


void print_help() {
   printf("Usage: ./main $dimensions $gridpoints_per_dimension $beta $b $iterations $seed\n");
   printf("The program will calculate the energy and magnetisation of a markov chain using the specified parameters\n");
   printf("and store them in seperate files (*.edat, *.mdat) with the filenames being an underline concatenation\n");
   printf("of the input parameters");
}


void parse_parameters(int argc, char** argv, int* d, int* n, double* beta, double* b, int* iterations, int* seed) {
    if (argc < 7) { fprintf(stderr, "Fatal error, %d parameters expected, got %d\n", 6, argc - 1); print_help(); exit(-1);}
    *d = atoi(argv[1]);
    *n = atoi(argv[2]);
    *beta = atof(argv[3]);
    *b= atof(argv[4]);
    *iterations = atoi(argv[5]);
    *seed = atoi(argv[6]);
    printf("Got: d=%d, n=%d, beta=%f, b=%f, iterations=%d, seed=%d\n", *d, *n, *beta, *b, *iterations, *seed);
}

int params_to_filename(int d, int n, double beta, double b, int iterations, int seed, int ft, char* filename) {
    if (ft == 0) {
        sprintf(filename, "%d_%d_%f_%f_%d_%d.edat", d, n, beta, b, iterations, seed);
    }
    else if (ft == 1) {
        sprintf(filename, "%d_%d_%f_%f_%d_%d.mdat", d, n, beta, b, iterations, seed);
    }
    else {
        return -1;
    }
    return 0;
}


int main(int argc, char** argv) {
    int d;
    int n;
    double beta;
    double b;
    int iterations;
    int seed;
    char* grid;
    char fn[PATH_MAX];
    int* magnetisation;
    double* energies;

    parse_parameters(argc, argv, &d, &n, &beta, &b, &iterations, &seed);
    rlxd_init(1, seed);
    if (alloc_grid(d, n, &grid) != 0) { fprintf(stderr, "Fatal error"); return -1; }
    if (!(magnetisation = (int*)malloc(iterations * sizeof(int)))) { fprintf(stderr, "Fatal error"); return -2; }
    if (!(energies = (double*)malloc(iterations * sizeof(double)))) { fprintf(stderr, "Fatal error"); return -3; }

    initialise_hot(d, n, grid);
    metro(grid, d, n, beta, b, iterations, magnetisation, energies);

    if (params_to_filename(d, n, beta, b, iterations, seed, 0, fn)) { fprintf(stderr, "This should never be reached"); }
    FILE* fd = fopen(fn, "w");
    for (int i = 0; i < iterations; i++)
    {
     fprintf(fd, "%d\t%f\n", i, energies[i]);
    }
    fclose(fd);

    if (params_to_filename(d, n, beta, b, iterations, seed, 1, fn)) { fprintf(stderr, "This should never be reached"); }
    fd = fopen(fn, "w");
    for (int i = 0; i < iterations; i++)
    {
     fprintf(fd, "%d\t%d\n", i, magnetisation[i]);
    }
    fclose(fd);
}

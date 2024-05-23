#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "statistics.h"
#include "metropolis.h"
#include "grid.h"
#include "ranlux.h"

/*
 * Calculates the transition probability t(s', 0) for a 1x4 grid. writes in file fd with format |s'|p|p_l|pu|
 *
 * @param iterations    number of iterations
 * @param fd            file to write the results in
*/
void metro_t0s(int iterations, FILE* fd, int d, int n, double beta, double b) { //
    char* grid;
    alloc_grid(d, n, &grid);

    int v = ipow(n, d);
    double* prng_nmbrs;
    prng_nmbrs = (double*)malloc(v * sizeof(double));


    int bin_nums = ipow(2, n);
    double bins[bin_nums];
    double bins_l[bin_nums];
    double bins_u[bin_nums];
    memset(bins, 0, sizeof(double) * bin_nums);
    memset(bins_l, 0, sizeof(double) * bin_nums);
    memset(bins_u, 0, sizeof(double) * bin_nums);
    for (int i = 0; i < iterations; i++) {
        *grid = 0x00;
        metro_iter_optimised(grid, d, n, v, beta, b, prng_nmbrs);
        bins[(unsigned int)*grid]+=1;
    }
    double p;
    double test = 0;
    for (int i = 0; i < bin_nums; i++) {
        p = bins[i] / iterations;
        estimate_confidece_interval(1, p, bins_l + i, bins_u + i, 1, iterations);
        fprintf(fd, "%d\t%.6e\t%.6e\t%.6e\n", i, p, bins_l[i], bins_u[i]);
        test += p;
    }
    fprintf(stderr, "test: %.6e\n", test);
    free(prng_nmbrs);
}

void elementary_update(char* grid, int i, int d, int n, double beta, double b, double* p, char* flipped_grid_buffer) {
    int v = ipow(n, d);
    double dh = delta_h(grid, d, n, v, beta, b, i);
    if (dh < 0) {*p = 0; *(p+1) = 1;}
    else {
        *p = 1-exp(-dh);
        *(p+1) = exp(-dh);
    }
    *flipped_grid_buffer = *grid;
    togglen(flipped_grid_buffer, d, n, i);
    return;
}

void calculate_Tss(FILE* fd, int d, int n, double beta, double b, char* start_config) {
    int v = ipow(n, d);
    // ipow(2, v) is number of possible configurations
    double Tss[ipow(2, v)];
    for (int j = 0; j < ipow(2,v); j++) {
        Tss[j] = 1;}
    char* grids[ipow(2, v)];
    for (int i = 0; i < ipow(2, v); i++) {
        alloc_grid(d, n, grids + i);
    }
    memcpy(grids[0], start_config, ((v-1)/8+1) * sizeof(char));
    double p[2];
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < ipow(2,i); j++) {
            elementary_update(grids[j], i, d, n, beta, b, p, grids[j + ipow(2, i)]);
            fprintf(stderr, "heele %d\t%d\t%.6e\t%.6e\n",i, j, p[0], p[1]);
            // grids[j] is unflipped config, grids[j+ipow(2,i)] is flipped config
            for (int k = 0; k < ipow(2,v - i - 1); k++) {
                fprintf(stderr, "writing into %d\t%d\n", j + k * ipow(2, i + 1), j + ipow(2, i) + k * ipow(2, i + 1));
                Tss[j + k * ipow(2, i + 1)] *= p[0];
                Tss[j + ipow(2, i) + k * ipow(2, i + 1)] *= p[1];
            }
        }
    }
    double test = 0;
    fprintf(stderr, "final config: \t");
    for (int j = 0; j < ipow(2,v); j++) {
        fprintf(stderr, "%.6e\n", Tss[j]);
        test+=Tss[j];
    }
    fprintf(stderr, "test %.6e\n", test);
    // sort the data and print in file
    int configuration_index;
    for (int j = 0; j < ipow(2,v); j++) {
        configuration_index = 0;
        for (int i = 0; i < v; i++) {
            configuration_index += getn(grids[j], d, n, i)*ipow(2,i);
        }
        fprintf(fd, "%d\t%.6e\n", configuration_index, Tss[j]);
    }
    return;
}

void main(int argc, char** argv) {
    int seed;
    if (argc < 2) { fprintf(stderr, "Forgot to enter seed? Will be set to 42069"); seed = 42069;}
    else { seed = atoi(argv[1]);}
    if (argc > 2) { fprintf(stderr, "WARING: too many arguments"); }
    rlxd_init(1, seed);

    int d = 1;
    int n = 4;
    double beta = 0.5;
    double b = - 0.1;

    int iter = 100000;
    char* fn = "t0s_n4.dat";
    FILE* fd = fopen(fn, "w");
    metro_t0s(iter, fd, d, n, beta, b);
    fclose(fd);
    char* fn_a = "t0s_n4_analytical.dat";
    FILE* fd_a = fopen(fn_a, "w");
    char start_conf = 0;
    calculate_Tss(fd_a, d, n, beta, b, &start_conf);
    fclose(fd_a);
}

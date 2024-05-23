/*\
 *
 * This module implements the metropolis algroithm
 *
\*/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ranlux.h"
#include "grid.h"
#include "metropolis.h"


// Calculate magnetisation
int calculate_magnetisation(
   char* grid,   /* Grid pointer */
   int d,        /* Number of dimensions */
   int n,        /* Number of points in each dimension */
   int v       /* pointer to privously defined grid volume*/) {

    int magnetisation = 0;
    for (int i=0; i < v; i++) {
        magnetisation += 2 *  getn(grid, d, n, i) - 1;
    }
    return magnetisation;
}

double calculate_energy(
   char* grid,   /* Grid pointer */
   int d,        /* Number of dimensions */
   int n,        /* Number of points in each dimension */
   int v,       /* pointer to privously defined grid volume*/
   double beta,   /*Beta parameter in the hamiltonian*/
   double b      /*b parameter in the hamiltonian*/) {
    double magnetisation_term = -b * (double)calculate_magnetisation(grid, d, n, v);
    double interaction_term = 0;
    // Interaction term of the hamiltonian
    for (int j = 0; j < v; j++) {
        int s = 2*getn(grid, d, n, j) - 1;
        int sum = 0;
        int coords[d];
        for (int i = 0; i < d; i++) {
            inv_idx(j, d, n, coords);
            coords[i] = (coords[i] + 1) % n;
            sum += getn(grid, d, n, idx(d, n, coords));
        }
        interaction_term += s * (2*sum - d);
    }
    interaction_term = -beta * interaction_term;
    return magnetisation_term + interaction_term;
}

// calculate Delta_H
double delta_h(
   char* grid,   /* Grid pointer */
   int d,        /* Number of dimensions */
   int n,        /* Number of points in each dimension */
   int v,       /* pointer to privously defined grid volume*/
   double beta,   /*Beta parameter in the hamiltonian*/
   double b,      /*b parameter in the hamiltonian*/
   int index  /* Coordinates to of where to calculate */) {

   int s = 2*getn(grid, d, n, index) - 1; /* go from {0,1} to {-1,1}*/
   int sum = 0; /* initialize sum term*/
   /* sum up neighbours */
   int coords[d];
#ifdef METROPOLIS_DEBUG
    inv_idx(index, d, n, coords);
    printf("calculating dh for point (%d, %d)\n", coords[0], coords[1]);
#endif
   for (int i = 0; i < d; i++) {
        inv_idx(index, d, n, coords);
        coords[i] = (coords[i] + 1) % n;
#ifdef METROPOLIS_DEBUG
        printf("accessing (%d, %d): %d\n", coords[0], coords[1], getn(grid, d, n, index));
#endif
        sum += 2 * getn(grid, d, n, idx(d, n, coords)) - 1;
        inv_idx(index, d, n, coords);
        coords[i] = (coords[i] + n - 1) % n;
#ifdef METROPOLIS_DEBUG
        printf("accessing (%d, %d): %d\n", coords[0], coords[1], getn(grid, d, n, index));
#endif
        sum += 2 * getn(grid, d, n, idx(d, n, coords)) - 1;
   };
   /* calculate Delta H*/
   double h_0 = 2.0f * beta * s * sum;
   double h_b = 2.0f * b * s;
   return h_0 + h_b;
}

// iteration of the metroplis algorithm in a memory optimised fashion
void metro_iter_optimised(
    char* grid,   /* Grid pointer */
    int d,        /* Number of dimensions */
    int n,        /* Number of points in each dimension */
    int v,        /* Volume of the grid */
    double beta,   /* Beta parameter in the hamiltonian*/
    double b,      /* b parameter in the hamiltonian*/
    double* preallocated_mem) /* The preallocated memory for the prndm numbers */
     {

    ranlxd(preallocated_mem, v);
    int dm = 0;
    for (int i = 0; i < v; i++) {
        double dh = delta_h(grid, d, n, v, beta, b, i);

        #ifdef METROPOLIS_DEBUG
        double energy_before = calculate_energy(grid, d, n, v, beta, b);
        togglen(grid, d, n, i);
        double dh2 = calculate_energy(grid, d, n, v, beta, b) - energy_before;
        togglen(grid, d, n, i);
        printf("METROPOLIS_DEBUG dh= %.6e  dh2= %.6e  |dh-dh2|= %.6e\n",dh,dh2,fabs(dh-dh2));
        #endif /* METROPOLIS_DEBUG */

        // if (dh < 0) { continue; }
        if (expf(-dh) > preallocated_mem[i]) {
            togglen(grid, d, n, i);
            // The difference in magnetisation is directly calculated from
            // the toggled grid point
            // *delta_magnetisation += 2 * getn(grid, d, n, i) - 1;
            // *new_energy += dh;
        }
    }
    return;
}

void metro(
    char* grid,     /* Grid pointer*/
    int d,          /* number of dimensions*/
    int n,          /* number of points i eaxh dimension*/
    double beta,     /* beta parmeter from the hamiltonian*/
    double b,        /* b parameter from the hamiltonian*/
    int iterations, /* number of iterations in one markov chain*/
    int* magnetisation, /* magnetisation pointer*/
    double* energy) {    /* energy pointer*/

    int v = ipow(n, d);
    double* prng_nmbrs;
    prng_nmbrs = (double*)malloc(v * sizeof(double));
    energy[0] = calculate_energy(grid, d, n, v, beta, b);
    magnetisation[0] = calculate_magnetisation(grid, d, n, v);
    for (int i = 1; i < iterations; i++)
    {
        metro_iter_optimised(grid, d, n, v, beta, b, prng_nmbrs);
        energy[i] = calculate_energy(grid, d, n, v, beta, b);
        magnetisation[i] = calculate_magnetisation(grid, d, n, v);
    }
    free(prng_nmbrs);
}

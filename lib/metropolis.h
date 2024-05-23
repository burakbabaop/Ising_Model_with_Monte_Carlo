#ifndef METROPOLIS_H
#define METROPOLIS_H

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ranlux.h"
#include "grid.h"

/*
 * Calculates the magnetization of a grid.
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param v     Pointer to previously defined grid volume.
 * @return      Magnetization of the grid.
 */
int calculate_magnetisation(char* grid, int d, int n, int v);

/*
 * Calculates the energy of a grid using the Ising model.
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param v     Pointer to previously defined grid volume.
 * @param beta  Beta parameter in the Hamiltonian.
 * @param b     b parameter in the Hamiltonian.
 * @return      Energy of the grid.
 */
double calculate_energy(char* grid, int d, int n, int v, double beta, double b);

/*
 * Calculates the change in Hamiltonian for a given grid point.
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param v     Pointer to previously defined grid volume.
 * @param beta  Beta parameter in the Hamiltonian.
 * @param b     b parameter in the Hamiltonian.
 * @param index Coordinates to calculate the change for.
 * @return      Change in Hamiltonian.
 */
double delta_h(char* grid, int d, int n, int v, double beta, double b, int index);

/*
 * Performs one iteration of the Metropolis algorithm in a memory-optimized fashion.
 *
 * @param grid              Grid pointer.
 * @param d                 Number of dimensions.
 * @param n                 Number of points in each dimension.
 * @param v                 Volume of the grid.
 * @param beta              Beta parameter in the Hamiltonian.
 * @param b                 b parameter in the Hamiltonian.
 * @param preallocated_mem The preallocated memory for the pseudorandom numbers.
 */
void metro_iter_optimised(char* grid, int d, int n, int v, double beta, double b, double* preallocated_mem);

/*
 * Performs the Metropolis algorithm for a given number of iterations.
 *
 * @param grid          Grid pointer.
 * @param d             Number of dimensions.
 * @param n             Number of points in each dimension.
 * @param beta          Beta parameter in the Hamiltonian.
 * @param b             b parameter in the Hamiltonian.
 * @param iterations    Number of iterations in one Markov chain.
 * @param magnetisation Pointer to store magnetization.
 * @param energy        Pointer to store energy.
 */
void metro(char* grid, int d, int n, double beta, double b, int iterations, int* magnetisation, double* energy);

#endif  // METROPOLIS_H

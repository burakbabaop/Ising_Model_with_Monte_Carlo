#ifndef GRID_H
#define GRID_H

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ranlux.h"

/*
 * Calculates the nth power of x (all integers).
 *
 * @param x Base.
 * @param n Exponent.
 * @return Result of x raised to the power of n.
 */
int ipow(int x, int n);

/*
 * Allocates memory for a grid on the heap.
 *
 * @param d    Number of dimensions.
 * @param n    Number of points in each dimension.
 * @param grid Pointer to the grid pointer.
 * @return     0 on success, -1 if malloc failed.
 */
int alloc_grid(int d, int n, char** grid);

/*
 * Initializes a grid with all grid points set to 1.
 *
 * @param d    Number of dimensions.
 * @param n    Number of points in each dimension.
 * @param grid Grid pointer.
 */
void initialise_cold(int d, int n, char* grid);

/*
 * Initializes a grid with a pseudorandom distribution of 1's and 0's.
 *
 * @param d    Number of dimensions.
 * @param n    Number of points in each dimension.
 * @param grid Grid pointer.
 */
void initialise_hot(int d, int n, char* grid);

/*
 * Retrieves the value of a grid point.
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param index Index of the point to get.
 * @return      Value of the grid point (either 0 or 1).
 */
int getn(char* grid, int d, int n, int index);

/*
 * Sets a grid point to 1.
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param index Index of the point to set.
 */
void raisen(char* grid, int d, int n, int index);

/*
 * Sets a grid point to 0.
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param index Index of the point to set.
 */
void lowern(char* grid, int d, int n, int index);

/*
 * Toggles a grid point.
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param index Index of the point to toggle.
 */
void togglen(char* grid, int d, int n, int index);

/*
 * Sets a grid point to a specific value (either 0 or 1).
 *
 * @param grid  Grid pointer.
 * @param d     Number of dimensions.
 * @param n     Number of points in each dimension.
 * @param index Index of the point to set.
 * @param value The value to set (either 0 or 1).
 */
void setn(char* grid, int d, int n, int index, char value);

/*
 * Calculates coordinates from index.
 *
 * @param i      Index.
 * @param d      Number of dimensions.
 * @param n      Number of points in each direction.
 * @param coords Coordinate pointer.
 */
void inv_idx(int i, int d, int n, int* coords);

/*
 * Calculates index from coordinates.
 *
 * @param d      Number of dimensions.
 * @param n      Number of points in each direction.
 * @param coords Coordinate pointer.
 * @return       Calculated index.
 */
int idx(int d, int n, int* coords);

/*
 * Utility to print the grid.
 *
 * @param grid Grid pointer.
 * @param d    Number of dimensions.
 * @param n    Number of points in each direction.
 */
void print_grid(char* grid, int d, int n);

#endif  // GRID_H

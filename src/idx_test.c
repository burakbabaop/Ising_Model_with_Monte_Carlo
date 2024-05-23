/*
 * Test the idx and inv_idx function for a 3x3 grid
 */

#include <stdio.h>
#include "grid.h"

int main() {
    int N = 3;
    int D = 2;
    int coords[2];
    for (int i=0; i < ipow(N, D); i++) {
        inv_idx(i, D, N, coords);
        printf("linear index: %d\t coordinates: %d,%d\t recalc lin: %d\n", i, coords[0], coords[1], idx(D, N, coords));
    }
}

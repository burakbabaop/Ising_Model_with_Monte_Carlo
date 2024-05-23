/*
 *
 * Generates some prandom numbers using ranlux
 *
*/
#include <stdio.h>
#include "ranlux.h"

int main(int argc, char** argv) {
        // The first parameter is the luxury level i.e
        // the quality of the pseudo random numbers.
        // According to the documentation even the lowest
        // level is sufficient for large scale monte carlo
        // simulations
        rlxd_init(2, 12345);
        // Here the double precision prngs are stored
        double prndm_nmbrs[16];
        // Request 16 prandom numbers
        ranlxd(prndm_nmbrs, 16);
        // Voila, piece of cake
        for (int i=0; i < 16; i++) {
                printf("%d\t\%f", i, prndm_nmbrs[i]);
        }
        return 0;
}

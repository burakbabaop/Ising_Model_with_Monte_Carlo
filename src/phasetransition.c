/*
 * Calculate magnetisation and energy as a function of beta for varying b values
 *
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "grid.h"
#include "ranlux.h"
#include "metropolis.h"
#include "statistics.h"

void fatal() {
   fprintf(stderr, "Fatal error\n");
   exit(-1);
}

int main(int argc, char** argv) {
   int d = 2;
   int n = 100;
   int n_conf = 100000;
   int thermalisation_time = 1000;
   double min_beta = 0.1;
   double max_beta = 2;
   int beta_steps = 10;
   double bs[] = {0.01, 0.005, 0.001, 0.0005, 0.0001};
   int* magnetisations;
   double* energies;
   char* grid;

   rlxd_init(1, 9);
   if (alloc_grid(d, n, &grid) != 0) { fatal(); }
   if ((magnetisations = (int*)malloc(n_conf * sizeof(int))) == NULL) { fatal(); }
   if ((energies = (double*)malloc(n_conf * sizeof(double))) == NULL) { fatal(); }

   FILE *energy_fd, *magnetisation_fd;
   energy_fd = fopen(argv[1], "w");
   magnetisation_fd = fopen(argv[2], "w");
   for (double beta = min_beta; beta <= max_beta; beta += (max_beta - min_beta) / beta_steps) {
      fprintf(energy_fd, "%lf", beta);
      fprintf(magnetisation_fd, "%lf", beta);
      for (int i = 0; i < 5; i++) {
         initialise_hot(d, n, grid);
         metro(grid, d, n, beta, bs[i], n_conf, magnetisations, energies);

         fprintf(magnetisation_fd, "\t%lf", calculate_imean(&magnetisations[thermalisation_time], n_conf - thermalisation_time));
         fprintf(energy_fd, "\t%lf", calculate_mean(&energies[thermalisation_time], n_conf - thermalisation_time));
      }
      fprintf(energy_fd, "\n");
      fprintf(magnetisation_fd, "\n");
      printf("%lf < %lf < %lf\n", min_beta, beta, max_beta);
   }
   fclose(energy_fd);
   fclose(magnetisation_fd);
   free(energies);
   free(magnetisations);
   free(grid);
}

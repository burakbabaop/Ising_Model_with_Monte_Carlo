#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include "statistics.h"


void parse_parameters(int argc, char** argv, char** path, int* N, int* N_therm) {
    if (argc < 3) { fprintf(stderr, "Fatal error, %d parameters expected, got %d\n", 2, argc - 1); exit(-1);}
    *path = argv[1];
    *N = atoi(argv[2]);
    *N_therm = atoi(argv[3]);
    fprintf(stderr, "Got: %s\n", *path);
}

int main(int argc, char** argv) {
    int N;
    int excerpt_N;
    int N_therm;
    char* path;
    double* data;

    parse_parameters(argc, argv, &path, &excerpt_N, &N_therm);
    N = get_markov_length(path);
    data = (double*)malloc(N * sizeof(double));

    if (read_data(path, data) != 0) { return -1; }
    int w_bar = calculate_w_bar(data, excerpt_N, N_therm);
    printf("%d\t%lf\n", excerpt_N, calculate_serror2(data, excerpt_N, N_therm, w_bar));
    // Loop w from 1 to w_bar and write s to plot it
    // Append the thermalisation point to the filename:
    char* new_filename = (char*)malloc(PATH_MAX);
    memset(new_filename, 0, PATH_MAX);
    strncpy(new_filename, path, strlen(path) - 5);
    new_filename[strlen(new_filename)] = '_';
    sprintf(new_filename + strlen(new_filename), "%d.s", N_therm);
    strcat(new_filename, path + strlen(path) - 4);
    FILE* fd = fopen(new_filename, "w");
    for (int w = 0; w < 2 * w_bar; w += 1) {
         fprintf(fd, "%d\t%lf\n", w, calculate_s(data, excerpt_N, N_therm, w));
    }
    fclose(fd);
    free(data);
    free(new_filename);
}

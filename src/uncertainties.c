/*
 * Calculates the uncertainties using a bunch of replica files in a directory
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include "statistics.h"


void print_help() {
   printf("Usage ./uncertainities $dir_path $min_thermtime $max_thermtime\n");
   printf("Calculate the uncertainties of magnetisation and energy expectation values for varying thermalisation times\n");
}


int count_files_in_dir(const char* dir_path, const char* substring) {
    DIR* d;
    struct dirent *dir;
    int c = 0;
    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, substring)) { c++; }
        }
    }
    return c;
}

void parse_parameters(int argc, char** argv, char** dir_path, int* min_thermalisation_time, int* max_thermalisation_time) {
    if (argc < 4) { fprintf(stderr, "Fatal error, %d parameters expected, got %d\n", 3, argc - 1); print_help(); exit(-1);}
    *dir_path = argv[1];
    *min_thermalisation_time = atoi(argv[2]);
    *max_thermalisation_time = atoi(argv[3]);
}

void read_from_dir(const char* dir_path,  const char* filetype, int* number_of_edat_files, char** edat_fns) {
    DIR* d;
    struct dirent *dir;
    int index;
    d = opendir(dir_path);
    *number_of_edat_files = count_files_in_dir(dir_path, filetype);
    // fprintf(stderr, "%d\n", *number_of_edat_files);
    *edat_fns = (char*)malloc(*number_of_edat_files * PATH_MAX);
    memset(*edat_fns, 0, *number_of_edat_files * PATH_MAX);
    if (d) {
        index = 0;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, filetype)) {
                memcpy((*edat_fns + index * PATH_MAX), &dir->d_name, strlen(dir->d_name));
                index += 1;
            }
        }
    }
}

int main(int argc, char** argv) {
    DIR* d;
    FILE *fd1, *fd2;
    struct dirent *dir;
    char* dir_path;
    int min_thermalisation_time;
    int max_thermalisation_time;
    double energy_expectation[2];
    double magnetisation_expectation[2];

    parse_parameters(argc, argv, &dir_path,
          &min_thermalisation_time, &max_thermalisation_time);
    int number_of_edat_files, number_of_mdat_files;
    char *edat_fns, *mdat_fns;
    read_from_dir(dir_path, (const char*)".edat", &number_of_edat_files, &edat_fns);
    read_from_dir(dir_path, (const char*)".mdat", &number_of_mdat_files, &mdat_fns);
    // Switch to dataset directory to deal with c strings as less as possible
    chdir(dir_path);
    fd1 = fopen("energy_error_thermtime.dat", "w");
    fd2 = fopen("magnetisation_error_thermtime.dat", "w");
    for (int i = min_thermalisation_time; i < max_thermalisation_time; i += (max_thermalisation_time - min_thermalisation_time) / 100)
    {
       analyse_dataset(edat_fns, number_of_edat_files, i, energy_expectation);
       analyse_dataset(mdat_fns, number_of_mdat_files, i, magnetisation_expectation);
       fprintf(fd1, "%d\t%f\n", max_thermalisation_time - i, energy_expectation[1]);
       fprintf(fd2, "%d\t%f\n", max_thermalisation_time - i, magnetisation_expectation[1]);
       printf("%d < %d < %d\n", min_thermalisation_time, i, max_thermalisation_time);
    }
    free(edat_fns);
    free(mdat_fns);
    fclose(fd1);
    fclose(fd2);
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include "../lib/statistics.h"

void print_help() {
   printf("Usage: ./analysation $dir_path $thermalisation_time\n");
   printf("Calculate and print the magnetisation and energy expectation values\n");
   printf("Calculate and store the probablity distributions in *.eprop and *.mprop\n");
   printf("Calculate and store the autocorellation\n");
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

void parse_parameters(int argc, char** argv, char** dir_path, int* thermalisation_time) {
    if (argc < 3) { fprintf(stderr, "Fatal error, %d parameters expected, got %d\n", 2, argc - 1); print_help(); exit(-1);}
    *dir_path = argv[1];
    *thermalisation_time = atoi(argv[2]);
    printf("Got: %s %d\n", *dir_path, *thermalisation_time);
}

void read_from_dir(const char* dir_path,  const char* filetype, int* number_of_edat_files, char** edat_fns) {
    DIR* d;
    struct dirent *dir;
    int index;
    d = opendir(dir_path);
    *number_of_edat_files = count_files_in_dir(dir_path, filetype);
    fprintf(stderr, "%d\n", *number_of_edat_files);
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
    struct dirent *dir;
    char* dir_path;
    int thermalisation_time;
    double energy_expectation[2];
    double magnetisation_expectation[2];

    parse_parameters(argc, argv, &dir_path, &thermalisation_time);
    int number_of_edat_files, number_of_mdat_files;
    char *edat_fns, *mdat_fns;
    read_from_dir(dir_path, (const char*)".edat", &number_of_edat_files, &edat_fns);
    read_from_dir(dir_path, (const char*)".mdat", &number_of_mdat_files, &mdat_fns);
    // Switch to dataset directory to deal with c strings as less as possible
    chdir(dir_path);
    analyse_dataset(edat_fns, number_of_edat_files, thermalisation_time, energy_expectation);
    analyse_dataset(mdat_fns, number_of_mdat_files, thermalisation_time, magnetisation_expectation);
    printf("%f\t%f\n", energy_expectation[0], energy_expectation[1]);
    printf("%f\t%f\n", magnetisation_expectation[0], magnetisation_expectation[1]);

//calculate pdf and observables from pdf

    int N_BINS = 100;

    double MIN_E = -6600;
    double MAX_E = -5800;

    double* bins_e;
    bins_e = malloc(sizeof(double)*N_BINS);
    memset(bins_e, 0, sizeof(double)*N_BINS);

    double* bins_e_l;
    bins_e_l = malloc(sizeof(double)*N_BINS);

    double* bins_e_u;
    bins_e_u = malloc(sizeof(double)*N_BINS);
    double energy_x[N_BINS];
    char fn[PATH_MAX];
    sprintf(fn, "N_BIN%d.eprop", N_BINS);
    FILE* fd=fopen(fn, "w");
    bin_dataset(MIN_E, MAX_E, N_BINS, bins_e, number_of_edat_files, thermalisation_time, edat_fns, bins_e_l, bins_e_u, 4);

    double DE = (MAX_E-MIN_E)/N_BINS;
    for (int i = 0; i < N_BINS; i++) {
        energy_x[i] = MIN_E + DE*i;
        fprintf(fd, "%.6e\t%.6e\t%.6e\t%.6e\n", energy_x[i], bins_e[i]/DE, bins_e_l[i]/DE, bins_e_u[i]/DE);
    }
    fclose(fd);

    double energy_from_pdf[2] ={0,0};
    observable_from_pdf(MIN_E, MAX_E, bins_e, N_BINS, energy_from_pdf);
    fprintf(stderr, "Energy: %lf pm %lf \n", energy_from_pdf[0], energy_from_pdf[1]);

    double MIN_M = 5500;
    double MAX_M = 8500;

    double* bins_m;
    bins_m = malloc(sizeof(double)*N_BINS);
    memset(bins_m, 0, sizeof(double)*N_BINS);

    double* bins_m_l;
    bins_m_l = malloc(sizeof(double)*N_BINS);

    double* bins_m_u;
    bins_m_u = malloc(sizeof(double)*N_BINS);

    double mag_x[N_BINS];
    char fn_m[PATH_MAX];
    sprintf(fn_m, "N_BIN%d.mprop", N_BINS);
    FILE* fdm=fopen(fn_m, "w");
    bin_dataset(MIN_M, MAX_M, N_BINS, bins_m, number_of_mdat_files, thermalisation_time, mdat_fns, bins_m_l, bins_m_u, 10);

    double DM = (MAX_M-MIN_M)/N_BINS;
    for (int i = 0; i < N_BINS; i++) {
        mag_x[i] = MIN_M + DM * i;
        fprintf(fdm, "%.6e\t%.6e\t%.6e\t%.6e\n", mag_x[i], bins_m[i]/DM, bins_m_l[i]/DM, bins_m_u[i]/DM);
    }
    fclose(fdm);

    double mag_from_pdf[2] ={0,0};
    observable_from_pdf(MIN_M, MAX_M, bins_m, N_BINS, mag_from_pdf);
    fprintf(stderr, "Magnetization: %.6e pm %.6e \n", mag_from_pdf[0], mag_from_pdf[1]);

    free(bins_e);
    free(bins_m);

// calculate autocorrelation for observables

    analyse_autocorr(edat_fns, number_of_edat_files, thermalisation_time, "energy_autoc.dat");
    analyse_autocorr(mdat_fns, number_of_mdat_files, thermalisation_time, "mag_autoc.dat");

    gaussian_binning(MIN_E, MAX_E, N_BINS, number_of_edat_files, thermalisation_time, edat_fns, "gauss_hist_energy.dat");
    gaussian_binning(MIN_M, MAX_M, N_BINS, number_of_mdat_files, thermalisation_time, mdat_fns, "gauss_hist_mag.dat");

    free(edat_fns);
    free(mdat_fns);
}

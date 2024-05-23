/*\
 *
 * This module implements everything to calculate the observables and its errors
 *
\*/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "grid.h"
#include "statistics.h"


int get_markov_length(const char* filename) {
    int bytes;
    int counter;
    char* content;
    FILE* fd;
    fd = fopen(filename, "r");
    if(fd == NULL) {fprintf(stderr, "gotcha!");}
    fseek(fd, 0, SEEK_END);
    bytes = ftell(fd);
    rewind(fd);
    content = (char*)malloc(bytes);
    fread(content, 1, bytes, fd);
    counter = 0;
    for (int i = 0; i < bytes; i++) {
        if (content[i] == '\n') { counter+= 1; }
    }
    free(content);
    return counter;
}

// reads data from file
int read_data(const char* filename, /* filename as string*/
              double* data           /* data pointer*/) {
    FILE* data_file;
    #ifdef STATISTICS_DEBUG
    printf("opening file \n");
    #endif
    data_file = fopen(filename, "r");
    if (data_file == NULL) {fprintf(stderr, "Could not open file! \n"); return -1;}
    double buffer;
    int i;
    while (fscanf(data_file, "%d\t%lf\n", &i, &buffer) > 0) {
        #ifdef STATISTICS_DEBUG
        printf("index: %d \t value: %f \n", i, buffer);
        #endif
        *(data+i) = buffer;
    }
    fclose(data_file);
    return 0;
}
// calculate mean of some data
double calculate_mean(double* values, /* values to calculate the mean of*/
                     int len        /* number of values*/) {
    double sum = 0;
    for(int i = 0; i < len; i++) {
        sum += values[i];
    }
    return sum / (double)len;
}

double calculate_imean(int* values, /* values to calculate the mean of*/
                     int len        /* number of values*/) {
    double sum = 0;
    for(int i = 0; i < len; i++) {
        sum += values[i];
    }
    return sum / (double)len;
}

// calculate error of data
double calculate_error(double* values, /* values to calculate the mean of*/
                      int len        /* number of values*/) {
    double sum = 0;
    double buffer;
    double expect = calculate_mean(values, len);
    double denominator = (len-1)*len;
    for(int i = 0; i < len; i++) {
        buffer = values[i] - expect;
        buffer *= buffer;
        sum += buffer;
    }
    return sqrt(sum/denominator);
}

double calculate_variance(double* values, /* values to calculate the mean of*/
                      int len        /* number of values*/) {
    double sum = 0;
    double buffer;
    double expect = calculate_mean(values, len);
    double denominator = len-1;
    for(int i = 0; i < len; i++) {
        buffer = values[i] - expect;
        buffer *= buffer;
        sum += buffer;
    }
    return sum/denominator;
}

void binning(double min, double max, int n_bins, double* bins, double* values, int n_values, int n_replic) {
    float binsize = (max-min)/n_bins;
    unsigned int bin = 0;
    for(long i = 0; i < n_values; i++) {
            if ((values[i] > min)&&(values[i] < max)) {
            bin = (values[i]-min)/binsize;
            bins[bin] += 1.0/(n_values * n_replic);
        }
    }
    return;
}

void estimate_confidece_interval(double z, double bin, double* bin_l, double* bin_u, double binsize, double n) {
    double p = bin;
    double p_mid = 1.0 / (1 + z * z / n) * (p + 0.5 * z * z / n);
    double sqrtprt = sqrt(p * (1.0-p) / n + (z * z) / (4.0 * n * n));
    double factor = z / (1 + z * z / n);
    double p_pm = sqrtprt * factor;
    *bin_u = (p_mid + p_pm);
    *bin_l = (p_mid - p_pm);
    return;
}

void bin_dataset(double min,
                 double max,
                 int n_bins,
                 double* bins,
                 int n_replica,
                 int n_thermal,
                 char* filenames,
                 double* bins_l,
                 double* bins_u,
                 double autocorr_time) {
    int markov_length = get_markov_length(filenames);
    double* values;
    values = malloc(sizeof(double)*(markov_length));
    for(int i = 0; i < n_replica; i++) {
        read_data(filenames + i*PATH_MAX, values);
        binning(min, max, n_bins, bins, values + n_thermal, markov_length -n_thermal, n_replica);
    }
    double test = 0;
    for (int i = 0; i < n_bins; i++) {
        estimate_confidece_interval(1. , bins[i], bins_l + i, bins_u + i, abs(values[1] - values[0]), markov_length*n_replica / sqrt(autocorr_time));
        test += bins[i];
    }
    printf("test for normality: %f\n", test);
    free(values);
    return;
}


void observable_from_pdf(double min,    /* minimal value for observable*/
                         double max,    /* maximal value for observable*/
                         double* pdf_x, /* pdf for the observable*/
                         int n_bins,    /* length of the array; number of bins*/
                         double* values /* buffer for results*/) {
    double px;
    double pxx;
    double dx = (max-min)/(float)n_bins;
    for(int i=0; i < n_bins; i++) {
        px = pdf_x[i] * (min + i * dx);
        pxx = px * (min + i * dx);
        values[0] += px;
        values[1] += pxx;
    }
    values[1] = sqrt(values[1] - values[0] * values[0]);
    return;
}

double calculate_s(
    double* data, /* Data to calculate serror of */
    int N /* Number of Datapoints */,
    int N_therm, /* Thermalisation point */
    int W /* Summation Window */) {

    double mean = calculate_mean(data + N_therm, N - N_therm);
    double sum = 0.0;
    for (int x = N_therm; x < N; x++) {
        for (int y = (x - W) < N_therm ? N_therm : x - W; y < ((x + W) > N ? N : x + W); y++) {
            sum += (data[x] - mean) * (data[y] - mean);
        }
    }
    sum /= (N - N_therm);
    sum /= (N - N_therm);
    return sum;
}

int calculate_w_bar(
    double* data, /* Data to calculate serror of */
    int N, /* Number of Datapoints */
    int N_therm /* Thermalisation point */) {

    for (int w = 1; w < 100; w++) {
        double s1 = calculate_s(data, N, N_therm, w);
        double s2 = calculate_s(data, N, N_therm, w + 1);
        if (fabs((s2 - s1) / s1) < 0.05) { return w; }
    }
    // This should *never* be reached
    return -1.0;
}

double calculate_serror(
    double* data, /* Data to calculate serror of */
    int N, /* Number of Datapoints */
    int N_therm/* Thermalisation point */) {

    return sqrt(calculate_s(data, N, N_therm, calculate_w_bar(data, N, N_therm)));
}

double calculate_serror2(
    double* data, /* Data to calculate serror of */
    int N, /* Number of Datapoints */
    int N_therm, /* Thermalisation point */
    int W_bar /* W bar */) {

    return sqrt(calculate_s(data, N, N_therm, W_bar));
}


void analyse_dataset(char* filenames, int n_replica,  int n_thermal, double* values) {
    double* data;
    double* energy_means;

    // Just take the first of all filenames to determine the markov length;
    // we assume that all replicas have the same length
    int markov_length = get_markov_length(filenames);
    // int markov_length = 1000;
    energy_means = malloc(sizeof(double)*n_replica);
    data = malloc(sizeof(double) * markov_length);
    for (int i = 0; i < n_replica; i++) {
        #ifdef STATISTICS_DEBUG
        printf("%s \n", filenames + i * PATH_MAX);
        #endif
        read_data(filenames + i * PATH_MAX, data);
        energy_means[i] = calculate_mean(data + n_thermal, markov_length - n_thermal);
    }
    double observable = calculate_mean(energy_means, n_replica);
    double observable_error = calculate_error(energy_means, n_replica);
    free(energy_means);
    values[0] = observable;
    values[1] = observable_error;
    free(data);
    return;
}

void autocorrelation(double* values, int len, double* autocorr_buffer) {
    double mean = calculate_mean(values, len);
    double variance = calculate_variance(values, len);
    double diff[len];
    for (int i = 0; i < len; i++) {
        diff[i] = values[i] - mean;
    }
    double auto_j;
    for (int j = 0; j < len; j++) {
        auto_j = 0;
        for (int i = 0; i < len-j; i++) {
            auto_j += diff[i] * diff[i+j];
        }
        autocorr_buffer[j] = auto_j / ((len-j) * variance); 
    }
    return;
}

void analyse_autocorr(char* filenames, int n_replica, int n_thermal, char* output_filename) {
    // Just take the first of all filenames to determine the markov length;
    // we assume that all replicas have the same length
    int markov_length = get_markov_length(filenames);

    double* data = malloc(sizeof(double) * markov_length);
    double* auto_j = malloc(sizeof(double) * (markov_length));
    double* auto_j_sqr = malloc(sizeof(double) * (markov_length));
    double* err_buffer = malloc(sizeof(double) * markov_length);
    double* autocorr_buffer = malloc(sizeof(double) * markov_length);

    memset(err_buffer, 0, sizeof(double) * markov_length);
    memset(autocorr_buffer, 0, sizeof(double) * markov_length);
    memset(data, 0, sizeof(double) * markov_length);
    memset(auto_j, 0, sizeof(double) * markov_length);
    memset(auto_j_sqr, 0, sizeof(double) * markov_length);
    // calculate autocorrelation mean for each time
    for (int i = 0; i < n_replica; i++) {
        read_data(filenames + i * PATH_MAX, data);
        autocorrelation(data + n_thermal, markov_length - n_thermal, auto_j);
        for (int j = 0; j < markov_length - n_thermal; j++) {
            autocorr_buffer[j] += auto_j[j] / n_replica;
            auto_j_sqr[j] += auto_j[j] * auto_j[j] / n_replica;
        }
    }
    // calculte error for each time
    FILE* fd = fopen(output_filename, "w");
    for (int j = 0; j < markov_length - n_thermal; j++) {
        err_buffer[j] = sqrt((auto_j_sqr[j] - autocorr_buffer[j] * autocorr_buffer[j]) / ((n_replica - 1)));
        fprintf(fd, "%d\t%.6e\t%.6e\n", j, autocorr_buffer[j], err_buffer[j]);
    }
    fclose(fd);

    free(err_buffer);
    free(autocorr_buffer);
    free(data);
    free(auto_j);
    free(auto_j_sqr);
    return;
}

void gaussian_binning(double min,
                      double max,
                      int n_bins,
                      int n_replica,
                      int n_thermal,
                      char* filenames,
                      char* output_filename) {
    // Just take the first of all filenames to determine the markov length;
    // we assume that all replicas have the same length
    int markov_length = get_markov_length(filenames);

    double* data = malloc(sizeof(double) * markov_length);
    double* bins_j = malloc(sizeof(double) * n_bins);
    double* bins_j_sqr = malloc(sizeof(double) * n_bins);
    double* err_buffer = malloc(sizeof(double) * n_bins);
    double* bin_buffer = malloc(sizeof(double) * n_bins);
    memset(data, 0, sizeof(double) * markov_length);
    memset(err_buffer, 0, sizeof(double) * n_bins);
    memset(bin_buffer, 0, sizeof(double) * n_bins);
    memset(bins_j_sqr, 0, sizeof(double) * n_bins);
    double test = 0;
    for (int i = 0; i < n_replica; i++) {
        memset(bins_j, 0, sizeof(double) * n_bins);
        read_data(filenames + i * PATH_MAX, data);
        binning(min, max, n_bins, bins_j, data + n_thermal, markov_length - n_thermal, 1);
        for (int j = 0; j < n_bins; j++) {
            bin_buffer[j] += bins_j[j] / n_replica;
            test += bins_j[j] / n_replica;
            bins_j_sqr[j] += bins_j[j] * bins_j[j] / n_replica;
        }
    }
    double dx = (max - min) / n_bins;
    // calculte error for each time
    FILE* fd = fopen(output_filename, "w");
    for (int j = 0; j < n_bins; j++) {
        err_buffer[j] = sqrt((bins_j_sqr[j] - bin_buffer[j] * bin_buffer[j]) / ((n_replica - 1)));
        fprintf(fd, "%.6e\t%.6e\t%.6e\n", (min + dx * j), bin_buffer[j] / dx, err_buffer[j] / dx);
    }
    printf("test: %f\n", test);
    fclose(fd);

    free(err_buffer);
    free(bin_buffer);
    free(data);
    free(bins_j);
    free(bins_j_sqr);
    return;
}

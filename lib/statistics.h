#ifndef STATISTICS_H
#define STATISTICS_H

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*
 * Reads data from a file and populates an array.
 *
 * @param filename  Path to data file 
 * @return          The length of the markov chain stored
 */
int get_markov_length(const char* filename);
/*
 * Reads data from a file and populates an array.
 *
 * @param filename  The name of the file to read data from.
 * @param data      Pointer to the array where data will be stored.
 * @return          0 on success, -1 on failure (e.g., file not found).
 */
int read_data(const char* filename, double* data);

/*
 * Calculates the mean of an array of values.
 *
 * @param values    Pointer to the array of values.
 * @param len       Number of values in the array.
 * @return          The mean of the values.
 */
double calculate_mean(double* values, int len);

/*
 * Calculates the mean of an array of integer values.
 *
 * @param values    Pointer to the array of values.
 * @param len       Number of values in the array.
 * @return          The mean of the values.
 */
double calculate_imean(int* values, int len);

/*
 * Calculates the error of an array of values.
 *
 * @param values    Pointer to the array of values.
 * @param len       Number of values in the array.
 * @return          The calculated error of the values.
 */
double calculate_error(double* values, int len);

/*
 * Analyzes a dataset of Markov chains and calculates observables.
 *
 * @param filenames         Array of strings containing filenames.
 * @param n_replica         Number of replicas.
 * @param n_thermal         Thermalization time.
 * @param values            Pointer to E(X) and Err(X) to be populated.
 */
void analyse_dataset(char* filenames, int n_replica, int n_thermal, double* values);

void binning(double min, double max, int n_bins, double* bins, double* values, int n_values, int n_replic);

/*



*/
void bin_dataset(double min, double max, int n_bins, double* bins, int n_replica, int n_thermal, char* filenames, double* bins_l, double* bins_u, double autocorrelation_time);

/*
 * Expected value from observable (including std) from pdf
 *
 * @param minimal value for observable
 * @param maximal value for observable
 * @param pdf for the observable
 * @param length of the array
 * @param buffer for results
*/
void observable_from_pdf(double min, double max, double* pdf_x, int n_bins, double* values);

/*
 * Calculate autocorrelation and error via the replica methode
 *
 * @param filenames         string containing filenames; each filename is separated MAX_PATH from the next
 * @param n_replica         number of replicas
 * @param n_thermal         thermalization time
 * @param output_filename   output filename as string
*/
void analyse_autocorr(char* filenames, int n_replica, int n_thermal, char* output_filename);

/*
 * Calculate ghistogram with gaussian error
 *
 * @param min                      minimum value in pdf
 * @param max                      maximum value in pdf
 * @param n_bins                   number of bins
 * @param n_replica                number of replicas
 * @param n_thermal                thermalization time
 * @param filenames                filenames of input files
 * @param output_filename          file to write to
*/

void gaussian_binning(double min,
                      double max,
                      int n_bins,
                      int n_replica,
                      int n_thermal,
                      char* filenames,
                      char* output_filename);

void estimate_confidece_interval(double z, double bin, double* bin_l, double* bin_u, double binsize, double n);

double calculate_s(double* data, int N, int N_therm, int W);
int calculate_w_bar(double* data, int N, int N_therm);
double calculate_serror(double* data, int N, int N_therm);
double calculate_serror2(double* data, int N, int N_therm, int W_bar); 
#endif  // STATISTICS_H

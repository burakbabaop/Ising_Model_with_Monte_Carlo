/*\
 *
 * This module implements the ising model data file (imd) format
 * and utility functions to read and write imd files
 *
\*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imdf.h"
#include "grid.h"


int imdf_allocate(
    struct imdf *frame,
    int N,
    int D,
    int replicas,
    int replica_length) {

    frame->N = N;
    frame->D = D;
    frame->V = ipow(N, D);
    frame->replicas = replicas;
    frame->replica_length = replica_length;
    int bytes_per_grid = imdf_get_bytes_per_grid(frame);
    frame->data = (char*)malloc(bytes_per_grid * replica_length * replicas);
}

int imdf_free(
    struct imdf *frame) {

    free(frame->data);
}

void imdf_store_grid(
    struct imdf *frame,
    int replica_number,
    int grid_number,
    char* grid) {

    int bytes_per_grid = imdf_get_bytes_per_grid(frame);
    int offset = bytes_per_grid * grid_number +
                 imdf_get_bytes_per_replica(frame) * replica_number;
    memcpy(&(frame->data)[offset], grid, bytes_per_grid);
}

int imdf_get_bytes_per_grid(
    struct imdf *frame) {

    int bytes_in_xdim = (frame->N - 1) / 8 + 1;
    int bytes = ipow(frame->N, frame->D - 1) * bytes_in_xdim;
}

int imdf_get_bytes_per_replica(
    struct imdf *frame) {

    return imdf_get_bytes_per_grid(frame) * frame->replica_length;
}

int imdf_write_to_file(
    struct imdf *frame,
    char* filename) {

    FILE* fd = fopen(filename, "wb");
    if (fd == NULL) { return -1; }
    int bytes_per_grid = imdf_get_bytes_per_grid(frame);
    fwrite(&frame->N,                sizeof(frame->N),               1, fd);
    fwrite(&frame->D,                sizeof(frame->D),               1, fd);
    fwrite(&frame->V,                sizeof(frame->V),               1, fd);
    fwrite(&frame->replicas,         sizeof(frame->replicas),        1, fd);
    fwrite(&frame->replica_length,   sizeof(frame->replica_length),  1, fd);
    fwrite(frame->data, sizeof(char), bytes_per_grid * frame->replica_length * frame->replicas, fd);
 }

/*\
 *
 * This module implements the ising model data file (imd) format
 * and utility functions to read and write imd files
 *
\*/

struct imdf {
    int N;
    int D;
    int V;
    int replicas;
    int replica_length;
    char* data;
};

int imdf_allocate(
    struct imdf *frame,
    int N,
    int D,
    int replicas,
    int replica_length);

int imdf_free(
    struct imdf *frame);

void imdf_store_grid(
    struct imdf *frame,
    int replica_number,
    int grid_number,
    char* grid);

int imdf_get_bytes_per_grid(
    struct imdf *frame);


int imdf_get_bytes_per_replica(
    struct imdf *frame);

int imdf_write_to_file(
    struct imdf *frame,
    char* filename);

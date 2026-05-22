#include <stdio.h>
#include <stdlib.h>

#include "tkcm.h"

#define ROWS 4310
#define COLS 166

void alloc_time_series(Opts *opts)
{
    opts->ts = calloc(opts->L, sizeof(double));

    opts->ref_ts = calloc(opts->d, sizeof(double *));

    for (int i = 0; i < opts->d; ++i) {
        opts->ref_ts[i] =
            calloc(opts->L, sizeof(double));
    }
}

void free_time_series(Opts *opts)
{
    for (int i = 0; i < opts->d; ++i) {
        free(opts->ref_ts[i]);
    }

    free(opts->ref_ts);
    free(opts->ts);
}

double **read_data(const char *filename)
{
    double **data =
        calloc(ROWS, sizeof(double *));

    for (int i = 0; i < ROWS; ++i) {
        data[i] =
            calloc(COLS, sizeof(double));
    }

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Could not open dataset: %s\n", filename);
        exit(-1);
    }

    for (int i = 0; i < ROWS; ++i) {

        for (int j = 0; j < COLS; ++j) {

            double value;

            if (fscanf(file, "%lf", &value) != 1) {
                printf(
                    "Error reading dataset at row %d col %d\n",
                    i,
                    j
                );

                fclose(file);
                exit(-1);
            }

            data[i][j] = value;
        }
    }

    fclose(file);

    return data;
}

void free_data(double **data)
{
    for (int i = 0; i < ROWS; ++i) {
        free(data[i]);
    }

    free(data);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tkcm.h"

#define ROWS 4310
#define COLS  166

void alloc_time_series(Opts *opts)
{
  opts->ts = calloc(opts->L, sizeof(double));
  opts->ref_ts = calloc(opts->d, sizeof(double *));
  for (int i = 0; i < opts->d; ++i) {
    opts->ref_ts[i] = calloc(opts->L, sizeof(double));
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


double **read_data()
{
  double **data = calloc(ROWS, sizeof(double *));
  for (int i = 0; i < ROWS; ++i) {
    data[i] = calloc(COLS, sizeof(double));
  }

  FILE *file = fopen("cl2fullLarge.dat","r");
  if (file == NULL) {
    printf("Could not open Chlorine dataset: cl2fullLarge.dat");
    exit(-1);
  }

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      double value;
      fscanf(file, "%lf", &value);
      data[i][j] = value;
    }
  }

  return data;
}


void free_data(double **data)
{
  for (int i = 0; i < 4310; ++i) {
    free(data[i]);
  }
  free(data);
}


int main()
{
  Opts opts = {
    .k = 5,
    .l = 72,
    .d = 3,
    .L = (ROWS - 288 * 3),
    .offset = -1,
  };

  alloc_time_series(&opts);
  double **data = read_data();

  for (int i = 0; i < ROWS; ++i) {
    opts.offset = mod(opts.offset + 1, opts.L);
    opts.ts[opts.offset] = data[i][0];
    opts.ref_ts[0][opts.offset] = data[i][1];
    opts.ref_ts[1][opts.offset] = data[i][2];
    opts.ref_ts[2][opts.offset] = data[i][3];

    if (i > opts.L) {
      double real_value = opts.ts[opts.offset];
      TKCM(&opts);

      printf("%lf\t%lf\n", real_value, opts.ts[opts.offset]);
    }
  }

  free_time_series(&opts);
  free_data(data);
}

#pragma once

#define mod(x,y) ((((x) % (y)) + (y)) % (y))

typedef enum {
    TKCM_MEAN,
    TKCM_WEIGHTED_INV,
    TKCM_WEIGHTED_EXP
} ImputeMode;

typedef struct {
  int k;
  int l;
  int d;
  int L;
  int offset;
  ImputeMode mode;
  double *ts;
  double **ref_ts;
  double alpha;
} Opts;


void TKCM(Opts *opts);

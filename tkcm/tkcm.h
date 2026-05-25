#pragma once

#define mod(x,y) ((((x) % (y)) + (y)) % (y))

typedef enum {
    TKCM_MEAN,
    TKCM_WEIGHTED_INV,
    TKCM_WEIGHTED_EXP
} ImputeMode;

typedef enum {
    DIST_L2,
    DIST_L1,
    DIST_DTW
} DistanceMetric;

typedef struct {
  int k;
  int l;
  int d;
  int L;
  int offset;
  ImputeMode mode;
  DistanceMetric metric;
  double *ts;
  double **ref_ts;
  double alpha;
} Opts;

double l2_distance(Opts *opts, int j);
double l1_distance(Opts *opts, int j);
double dtw_distance(Opts *opts, int j);

void TKCM(Opts *opts);

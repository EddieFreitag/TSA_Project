#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tkcm.h"

#define POS(row,col) (((nr_patterns+1)*(row)) + (col))

double l2_distance(Opts *opts, int j)
{
    double dist = 0.0;
    for (int i = 0; i < opts->d; ++i) {
        for (int x = 0; x < opts->l; ++x) {
            int pos = opts->offset + opts->l + j - 1 - x;
            double x1 = opts->ref_ts[i][mod(pos, opts->L)];
            double x2 = opts->ref_ts[i][mod(opts->offset - x, opts->L)];
            double diff = x1 - x2;
            dist += diff * diff;
        }
    }
    return sqrt(dist);
}

double l1_distance(Opts *opts, int j)
{
    double dist = 0.0;
    for (int i = 0; i < opts->d; ++i) {
        for (int x = 0; x < opts->l; ++x) {
            int pos = opts->offset + opts->l + j - 1 - x;
            double x1 = opts->ref_ts[i][mod(pos, opts->L)];
            double x2 = opts->ref_ts[i][mod(opts->offset - x, opts->L)];
            dist += fabs(x1 - x2);
        }
    }
    return dist;
}

double dtw_distance(Opts *opts, int j)
{
    int l = opts->l;
    double *dtw = calloc((l + 1) * (l + 1), sizeof(double));

    #define DTW(a,b) dtw[(a)*(l+1) + (b)]
    // Initialize DTW matrix
    for (int i = 0; i <= l; ++i) {
        for (int k = 0; k <= l; ++k) {
            DTW(i,k) = INFINITY;
        }
    }
    DTW(0,0) = 0.0;
    for (int x = 1; x <= l; ++x) {
        for (int y = 1; y <= l; ++y) {
            double cost = 0.0;
            for (int dim = 0; dim < opts->d; ++dim) {
                int pos1 = opts->offset + opts->l + j - x;
                int pos2 = opts->offset - y + 1;
                double v1 = opts->ref_ts[dim][mod(pos1, opts->L)];
                double v2 = opts->ref_ts[dim][mod(pos2, opts->L)];
                cost += fabs(v1 - v2);
            }
            double best = fmin(DTW(x-1,y), fmin(DTW(x,y-1), DTW(x-1,y-1)));
            DTW(x,y) = cost + best;
        }
    }
    double result = DTW(l,l);
    free(dtw);
    return result;
}

void TKCM(Opts *opts)
{
  int nr_patterns = opts->L - 2*opts->l + 1;
  double *M = calloc((opts->k+1)*(nr_patterns+1), sizeof(double));
  double *D = calloc(nr_patterns+1, sizeof(double));
  int *A = calloc(opts->k, sizeof(int));

  // step 1: compute pattern dissimilarities
  for (int j = 1; j <= nr_patterns; ++j) {
    if (opts->metric == DIST_L2) {
      D[j] = l2_distance(opts, j);
    } else if (opts->metric == DIST_L1) {
      D[j] = l1_distance(opts, j);
    } else if (opts->metric == DIST_DTW) {
      D[j] = dtw_distance(opts, j);
    }
  }

  // step 2.1: dynamic programming
  for (int j = 0; j <= opts->L-2*opts->l+1; ++j) {
    M[POS(0,j)] = 0;
    for (int i = 1; i <= opts->k; ++i) {
      if (i > j) {
        M[POS(i,j)] = INFINITY;
      } else {
        int pred = j-opts->l >= 0 ? j-opts->l : 0;
        M[POS(i,j)] = fmin(M[POS(i,j-1)], D[j]+M[POS(i-1,pred)]);
      }
    }
  }

  // step 2.2: backtracking
  int i = opts->k;
  int j = opts->L - 2*opts->l + 1;
  while (i > 0) {
    if (M[POS(i,j)] == M[POS(i,j-1)]) {
      --j;
    } else {
      A[i-1] = j;
      --i;
      j = j-opts->l >= 0 ? j-opts->l : 0;
    }
  }
  
  // step 3: impute missing value
  double weighted_sum = 0.0;
  double weight_total = 0.0;
  const double eps = 1e-8;

  for (int i = 0; i < opts->k; ++i) {
    int match_idx = A[i];
    int pos = opts->offset + opts->l + match_idx - 1;
    double value = opts->ts[mod(pos, opts->L)];
    double weight = 1.0;

    if (opts->mode == TKCM_MEAN) {
      weight = 1.0;
    } else if (opts->mode == TKCM_WEIGHTED_INV) {
      weight = 1.0 / (D[match_idx] + eps);
    } else if (opts->mode == TKCM_WEIGHTED_EXP) {
      weight = exp(-opts->alpha * D[match_idx]);
    }

    weighted_sum += weight * value;
    weight_total += weight;
  }
  
  opts->ts[opts->offset] = weighted_sum / weight_total;

  free(M);
  free(D);
  free(A);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tkcm.h"

#define ROWS 12

double s[ROWS] = {
  22.8, 21.4, 21.8, 23.1, 23.5, 22.8, 21.2, 21.9, 23.5, 22.8, 21.2, 21.9,
};
double r1[ROWS] = {
  16.5, 17.2, 17.8, 16.6, 15.8, 16.2, 17.4, 17.7, 15.3, 16.3, 17.1, 17.5
};
double r2[ROWS] = {
  20.3, 19.8, 18.6, 18.8, 20.0, 20.4, 19.8, 18.2, 20.1, 20.2, 19.9, 18.2
};

double *R[2] = { r1, r2 };


int main()
{
  Opts opts = {
    .k = 2,
    .l = 3,
    .d = 2,
    .L = ROWS,
    .ts = s,
    .ref_ts = R,
  };
  opts.offset = opts.L - 1;

  for (int i = ROWS-1; i < ROWS; ++i) {
    double real_value = opts.ts[opts.offset];
    TKCM(&opts);

    printf("%lf\t%lf\n", real_value, opts.ts[opts.offset]);
  }
}

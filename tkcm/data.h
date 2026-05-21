#ifndef DATA_H
#define DATA_H

#include "tkcm.h"

double **read_data();

void free_data(double **data);

void alloc_time_series(Opts *opts);

void free_time_series(Opts *opts);

#endif
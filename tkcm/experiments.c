#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "data.h"
#include "tkcm.h"

#define ROWS 4310
#define COLS 166



const char *mode_name(ImputeMode mode)
{
    switch(mode) {
        case TKCM_MEAN:
            return "mean";

        case TKCM_WEIGHTED_INV:
            return "weighted_inv";

        case TKCM_WEIGHTED_EXP:
            return "weighted_exp";
    }

    return "unknown";
}

const char *metric_name(DistanceMetric metric)
{
    switch(metric) {

        case DIST_L2:
            return "l2";

        case DIST_L1:
            return "l1";

        case DIST_DTW:
            return "dtw";
    }

    return "unknown";
}

void run_experiments(const char *data_file, const char *output_file)
{
    double **data = read_data(data_file);

    FILE *out = fopen(output_file, "w");

    if (!out) {
        printf("Could not open output file %s\n", output_file);
        exit(1);
    }

    fprintf(out,
        "mode,metric,k,l,d,alpha,index,real,pred,error\n");

    int ks[] = {3,5,7,9};
    int ls[] = {12,24,48,72,96};

    ImputeMode modes[] = {
        TKCM_MEAN,
        TKCM_WEIGHTED_INV,
        TKCM_WEIGHTED_EXP
    };

    DistanceMetric metrics[] = {
        DIST_L2,
        DIST_L1,
        DIST_DTW
    };

    for (int m = 0; m < 3; ++m) {
        
        for (int met = 0; met < 3; ++met) {

            for (int ki = 0; ki < 4; ++ki) {

                for (int li = 0; li < 5; ++li) {

                    Opts opts = {
                        .k = ks[ki],
                        .l = ls[li],
                        .d = 3,
                        .L = (ROWS - 288 * 3),
                        .offset = -1,
                        .mode = modes[m],
                        .metric = metrics[met],
                        .alpha = 0.5
                    };

                    alloc_time_series(&opts);

                    for (int i = 0; i < ROWS; ++i) {

                        opts.offset = mod(opts.offset + 1, opts.L);

                        opts.ts[opts.offset] = data[i][0];

                        opts.ref_ts[0][opts.offset] = data[i][1];
                        opts.ref_ts[1][opts.offset] = data[i][2];
                        opts.ref_ts[2][opts.offset] = data[i][3];

                        if (i > opts.L) {

                            double real_value =
                                opts.ts[opts.offset];

                            TKCM(&opts);

                            double pred =
                                opts.ts[opts.offset];

                            double error =
                                fabs(real_value - pred);

                            fprintf(out,
                                "%s,%s,%d,%d,%d,%lf,%d,%lf,%lf,%lf\n",
                                mode_name(opts.mode),
                                metric_name(opts.metric),
                                opts.k,
                                opts.l,
                                opts.d,
                                opts.alpha,
                                i,
                                real_value,
                                pred,
                                error
                            );
                        }
                    }

                    printf("Finished %s | mode=%s, metric=%s, k=%d, l=%d\n",
                        data_file,
                        mode_name(opts.mode),
                        metric_name(opts.metric),
                        opts.k,
                        opts.l
                    );

                    free_time_series(&opts);
                }
            }
        }
    }

    fclose(out);
    free_data(data);
}

void benchmark_distance_metrics(
    const char *data_file,
    const char *output_file)
{
    double **data = read_data(data_file);

    FILE *out = fopen(output_file, "w");

    if (!out) {
        printf("Could not open %s\n", output_file);
        exit(1);
    }

    fprintf(out,
        "dataset,metric,k,l,total_time,"
        "avg_time_per_imputation,nr_imputations\n");

    int ks[] = {1,3,5,7};
    int ls[] = {6,12,24,48,72};

    DistanceMetric metrics[] = {
        DIST_L2,
        DIST_L1,
        DIST_DTW
    };

    for (int mi = 0; mi < 3; ++mi) {

        for (int ki = 0; ki < 4; ++ki) {

            for (int li = 0; li < 5; ++li) {

                Opts opts = {
                    .k = ks[ki],
                    .l = ls[li],
                    .d = 3,
                    .L = (ROWS - 288 * 3),
                    .offset = -1,
                    .mode = TKCM_MEAN,
                    .metric = metrics[mi],
                    .alpha = 0.5
                };

                alloc_time_series(&opts);

                int nr_imputations = 0;

                clock_t start = clock();

                for (int i = 0; i < ROWS; ++i) {

                    opts.offset =
                        mod(opts.offset + 1,
                            opts.L);

                    opts.ts[opts.offset] =
                        data[i][0];

                    opts.ref_ts[0][opts.offset] =
                        data[i][1];

                    opts.ref_ts[1][opts.offset] =
                        data[i][2];

                    opts.ref_ts[2][opts.offset] =
                        data[i][3];

                    if (i > opts.L) {

                        TKCM(&opts);

                        nr_imputations++;
                    }
                }

                clock_t end = clock();

                double total_time =
                    (double)(end - start)
                    / CLOCKS_PER_SEC;

                double avg_time =
                    total_time / nr_imputations;

                fprintf(out,
                    "%s,%s,%d,%d,%lf,%lf,%d\n",
                    data_file,
                    metric_name(opts.metric),
                    opts.k,
                    opts.l,
                    total_time,
                    avg_time,
                    nr_imputations
                );

                printf(
                    "Benchmark complete | "
                    "metric=%s k=%d l=%d "
                    "time=%lf sec\n",
                    metric_name(opts.metric),
                    opts.k,
                    opts.l,
                    total_time
                );

                free_time_series(&opts);
            }
        }
    }

    fclose(out);

    free_data(data);
}

int main()
{
    //run_experiments("cl2fullLarge.dat", "results.csv");
    //printf("Finished first experiment\n");
    //run_experiments("cl2fullLarge_shifted.dat", "results_shifted.csv");
    //printf("Finished shifted experiment\n");

    printf("Starting benchmark of distance metrics\n");
    benchmark_distance_metrics("cl2fullLarge.dat", "benchmark_results.csv");
    printf("Finished benchmark of distance metrics\n");

    return 0;
}
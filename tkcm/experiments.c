#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

void run_experiments(const char *data_file, const char *output_file)
{
    double **data = read_data(data_file);

    FILE *out = fopen(output_file, "w");

    if (!out) {
        printf("Could not open output file %s\n", output_file);
        exit(1);
    }

    fprintf(out,
        "mode,k,l,d,alpha,index,real,pred,error\n");

    int ks[] = {3,5,7,9};
    int ls[] = {12,24,48,72,96};

    ImputeMode modes[] = {
        TKCM_MEAN,
        TKCM_WEIGHTED_INV,
        TKCM_WEIGHTED_EXP
    };

    for (int m = 0; m < 3; ++m) {

        for (int ki = 0; ki < 4; ++ki) {

            for (int li = 0; li < 5; ++li) {

                Opts opts = {
                    .k = ks[ki],
                    .l = ls[li],
                    .d = 3,
                    .L = (ROWS - 288 * 3),
                    .offset = -1,
                    .mode = modes[m],
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
                            "%s,%d,%d,%d,%lf,%d,%lf,%lf,%lf\n",
                            mode_name(opts.mode),
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

                printf("Finished %s | mode=%s, k=%d, l=%d\n",
                    data_file,
                    mode_name(opts.mode),
                    opts.k,
                    opts.l
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
    run_experiments("cl2fullLarge.dat", "results.csv");
    printf("Finished first experiment\n");
    run_experiments("cl2fullLarge_shifted.dat", "results_shifted.csv");
    printf("Finished shifted experiment\n");

    return 0;
}
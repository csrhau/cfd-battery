#ifndef CCILK_MAIN_H
#define CCILK_MAIN_H

#define CCILK_DATASET "/temperature"

void simulate(double *data, double *next, size_t rows, size_t cols,
              double width, double depth, double nu, double sigma,
              int outrate, int timesteps, char *prefix);

void hdf5_output(double *data, int ts, char *prefix);

void print_usage(void);

#endif

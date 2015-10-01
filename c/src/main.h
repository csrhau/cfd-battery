#ifndef CCILK_MAIN_H
#define CCILK_MAIN_H

#define CCILK_DATASET "/temperature"

void simulate(double *data, double *next, size_t rows, size_t cols, int outrate, int timesteps);
void print_usage(void);

#endif

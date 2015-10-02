#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "main.h"

#define INDEX2D(row, col, rows, cols) ((row) * (cols) + (col))

#define GETOPTS "i:p:r:t:"
static char *progname;
static struct option long_opts[] = {
  {"infile", required_argument, NULL, 'i'},
  {"prefix", required_argument, NULL, 'p'},
  {"output-rate", required_argument, NULL, 'r'},
  {"timesteps", required_argument, NULL, 't'},
  {NULL, 0, NULL, 0}
};

int main(int argc, char *argv[]) {
  progname = argv[0];
  int show_usage = 0;
  int outrate = 1, timesteps = 17;
  char *infile = NULL, *prefix = NULL;
  int optc;
  while ((optc = getopt_long(argc, argv, GETOPTS, long_opts, NULL)) != -1) {
    switch (optc) {
      case 'i':
        if (infile != NULL) {
          free(infile);
        }
        infile = strdup(optarg);
        break;
      case 'p':
        if (prefix != NULL) {
          free(prefix);
        }
        prefix = strdup(optarg);
        break;
      case 'r':
        outrate = atoi(optarg);
        break;
      case 't':
        timesteps = atoi(optarg);
        if (timesteps < 1) {
          fprintf(stderr, "Invalid value for timesteps: %s, must be a number greater than 1.\n", optarg);
          show_usage = 1;
        }
        break;
      default:
          show_usage = 1;
    }
  }
  if (show_usage == 1 || optind < argc || infile == NULL || prefix == NULL) {
    print_usage();
    return EXIT_FAILURE;
  }
  // READ FILE
  hsize_t dims[2];
  hid_t file_id;
  if ((file_id = H5Fopen(infile, H5F_ACC_RDONLY, H5P_DEFAULT)) < 0) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (H5LTget_dataset_info(file_id, CCILK_DATASET, dims, NULL, NULL) < 0) {
    print_usage();
    return EXIT_FAILURE;
  }
  size_t rows = dims[0];
  size_t cols = dims[1];
  double *data = malloc(rows * cols * sizeof(double));
  double *next = malloc(rows * cols * sizeof(double));
  double width, depth, nu, sigma; 
  if (H5LTread_dataset_double(file_id, CCILK_DATASET, data) < 0
   || H5LTget_attribute_double(file_id, "/domain/", "width", &width) < 0 
   || H5LTget_attribute_double(file_id, "/domain/", "depth", &depth) < 0 
   || H5LTget_attribute_double(file_id, "/properties/", "nu", &nu) < 0 
   || H5LTget_attribute_double(file_id, "/properties/", "sigma", &sigma) < 0) {
    fprintf(stderr, "Encountered an issue reading the dataset\n");
    return EXIT_FAILURE;
  };

  printf("Width: %f dept %f\n", width, depth);

  simulate(data, next, rows, cols, width, depth, nu, sigma, outrate, timesteps, prefix);
  free(data);
  H5Fclose(file_id);
  return EXIT_SUCCESS;
}

void simulate(double *data,
              double *next, 
              size_t rows, 
              size_t cols, 
              double width, 
              double depth,
              double nu,
              double sigma,
              int outrate, 
              int timesteps,
              char *prefix){
  double tInit = 0;
  for (int i = 1; i < rows-1; i++) {
    for (int j = 1; j < cols-1; j++) {
       tInit +=  data[i * cols + j];
    }
  }

  // Initial Write
  hdf5_output(data, 0, rows, cols, prefix);

  const double dx = width / (cols - 1);
  const double dy = depth / (rows - 1);
  const double dt = sigma * dx * dy / nu;
  const double cx = nu * dt / (dx * dx);
  const double cy = nu * dt / (dy * dy); 
  for (int ts = 1; ts <= timesteps; ++ts) {
    // Do the simulationen
    for (int i = 1; i < rows-1; ++i) {
      for (int j = 1; j < cols-1; ++j) {
        const size_t center = INDEX2D(i, j, rows, cols);
        const size_t north = INDEX2D(i-1, j, rows, cols);
        const size_t west = INDEX2D(i, j-1, rows, cols);
        const size_t south = INDEX2D(i+1, j, rows, cols);
        const size_t east = INDEX2D(i, j+1, rows, cols);
        next[center] = data[center] 
          + cx * (data[west] - 2 * data[center] + data[east]) 
          + cy * (data[north] - 2 * data[center] + data[south]);
      }
    }

    // Reflect Boundaries
    // TOP
    for (int j = 0; j < cols; ++j) {
      const size_t top_outer = INDEX2D(0, j, rows, cols);
      const size_t top_inner = INDEX2D(1, j, rows, cols);
      next[top_outer] = 1; //next[top_inner];
    }
    // LEFT
    for (int i = 1; i < rows-1; ++i) {
      const size_t left_outer = INDEX2D(i, 0, rows, cols);
      const size_t left_inner = INDEX2D(i, 1, rows, cols);
      next[left_outer] = 1; // next[left_inner];
    }
    // BOTTOM
    for (int j = 0; j < cols; ++j) {
      const size_t bottom_outer = INDEX2D(rows-1, j, rows, cols);
      const size_t bottom_inner = INDEX2D(rows-2, j, rows, cols);
      next[bottom_outer] = 1; // next[bottom_inner];
    }
    // RIGHT
    for (int i = 1; i < rows-1; ++i) {
      const size_t right_outer = INDEX2D(i, cols-1, rows, cols);
      const size_t right_inner = INDEX2D(i, cols-2, rows, cols);
      next[right_outer] = 1; // next[right_inner];
    }

    // Calculate temp

    // Swap spaces
    double *temp = next;
    next = data;
    data = temp;

    if (ts % outrate == 0) {
      hdf5_output(data, ts, rows, cols, prefix);
    }
  }
  if (timesteps % outrate != 0) {
      // Final output
      hdf5_output(data, timesteps, rows, cols, prefix);
  }
}

void hdf5_output(double *data, int ts, int rows, int cols, char *prefix) {
  char buffer [MAX_FILENAME_LENGTH];
  snprintf(buffer, MAX_FILENAME_LENGTH, "%s-%d.h5", prefix, ts);
  printf("Outputting data for timestep %d to %s\n", ts, buffer);
  hid_t file_id = H5Fcreate(buffer, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  hsize_t dims[2] = {rows, cols};
  H5LTmake_dataset_double(file_id, CCILK_DATASET, 2, dims, data);
  H5Fclose (file_id);
}

void print_usage(void) {
  fprintf(stderr, "%s: A simple 2D simulation of the diffusion equation.\n", progname);
  fprintf(stderr, "Usage: %s [OPTIONS]\n", progname);
  fprintf(stderr, "  -i,  --infile            Path to input file       [required]\n"
                  "  -p,  --prefix            Output filename prefix   [required]\n"
                  "  -r,  --output-rate       Timesteps between output [optional]\n"
                  "  -t,  --timesteps         Simulation Timesteps     [optional]\n");
}

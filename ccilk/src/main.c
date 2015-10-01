#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "hdf5.h"

#include "main.h"

static char *progname;

#define GETOPTS "i:p:r:t:"
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
  return EXIT_SUCCESS;
}


void simulate(void) {
  printf("Whatevs\n");
}

void print_usage(void) {
  fprintf(stderr, "%s: A simple 2D simulation of the diffusion equation.\n", progname);
  fprintf(stderr, "Usage: %s [OPTIONS]\n", progname);
  fprintf(stderr, "  -i,  --infile            Path to input file       [required]\n"
                  "  -p,  --prefix            Output filename prefix   [required]\n"
                  "  -r,  --output-rate       Timesteps between output [optional]\n"
                  "  -t,  --timesteps         Simulation Timesteps     [optional]\n");
}

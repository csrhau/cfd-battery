import h5py
import argparse
import numpy as np

def process_arguments():
  parser = argparse.ArgumentParser(description="Heat Equation IC generator")
  parser.add_argument('-x', '--cols', type=int, default=31, help='Simulation columns')
  parser.add_argument('-y', '--rows', type=int, default=31, help='Simulation rows')
  parser.add_argument('-w', '--width', type=float, default=2, help='Simulation domain width')
  parser.add_argument('-d', '--depth', type=float, default=2, help='Simulation domain depth (height)')
  parser.add_argument('-s', '--sigma', type=float, default=.25, help='Sigma')
  parser.add_argument('-n', '--nu', type=float, default=.05, help='Nu')
  parser.add_argument('-g', '--generator', type=str, choices=['barbra', 'ones', 'hotcorner'], default='barbra')
  parser.add_argument("-of", "--outfile", type=str, required=True, help="Path to data file to write to")
  return parser.parse_args()

def ones_gen(rows, cols):
  data = np.ones((rows, cols))
  return data

def barbra_gen(rows, cols):
  data = np.ones((rows, cols))
  dx = 2.0 / (cols - 1)
  dy = 2.0 / (rows - 1)
  data[.5/dy:1/dy+1,.5/dx:1/dx+1] = 2
  return data

def hotcorner_gen(rows, cols):
  data = np.zeros((rows, cols))
  data[0:rows/3,0:cols/3] = 2
  return data


def main():
  args = process_arguments()

  if args.generator == 'barbra':
    data = barbra_gen(args.rows, args.cols)
  elif args.generator == 'ones':
    data = ones_gen(args.rows, args.cols)
  elif args.generator == 'hotcorner':
    data = hotcorner_gen(args.rows, args.cols)

  with h5py.File(args.outfile, 'w-') as of:
    dom = of.create_group('domain')
    dom.attrs['width'] = args.width
    dom.attrs['depth'] = args.depth
    prp = of.create_group('properties')
    prp.attrs['sigma'] = args.sigma
    prp.attrs['nu'] = args.nu
    ds = of.create_dataset('temperature', data=data) 
    ds.attrs['generator'] = np.string_(args.generator)

if __name__ == '__main__':
  main()

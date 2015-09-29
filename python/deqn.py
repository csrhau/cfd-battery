from __future__ import division, print_function
import h5py
import argparse
import numpy as np

def process_arguments():
  parser = argparse.ArgumentParser(description="Heat Equation IC generator")
  parser.add_argument("-if", "--infile", type=str, required=True, help="Path to input data file")
  parser.add_argument("-op", "--output_prefix", type=str, required=True, help="Initial portion of output filename")
  parser.add_argument('-i', '--write-interval', type=int, default=1, help='timestep interval to write state out')
  parser.add_argument('-t', '--timesteps', type=int, default=17, help='Timesteps to advance simulation by')
  return parser.parse_args()

def hdf5_output(data, t, prefix):
  with h5py.File("{}-{}.h5".format(prefix, t), 'w-') as of:
    ds = of.create_dataset('temperature', data=data) 

def main():
  args = process_arguments()
  with h5py.File(args.infile, 'r') as infile:
    data = infile['temperature'][...]
    nu = infile.attrs['nu']
    sigma = infile.attrs['sigma']
  ny, nx = data.shape
  nt = args.timesteps
  dx = 2.0/(nx-1)
  dy = 2.0/(ny-1)
  dt = sigma*dx*dy/nu

  hdf5_output(data, 0, args.output_prefix)
  for t in range(1, nt+1): 
    un = data.copy()
    data[1:-1,1:-1]=un[1:-1,1:-1]+nu*dt/dx**2*(un[1:-1,2:]-2*un[1:-1,1:-1]+un[1:-1,0:-2])\
                                 +nu*dt/dy**2*(un[2:,1:-1]-2*un[1:-1,1:-1]+un[0:-2,1:-1])
    data[0,:]=1
    data[-1,:]=1
    data[:,0]=1
    data[:,-1]=1

    if t % args.write_interval == 0:
      hdf5_output(data, t, args.output_prefix)
  if t % args.write_interval != 0:
    hdf5_output(data, t, args.output_prefix)

if __name__ == '__main__':
  main()

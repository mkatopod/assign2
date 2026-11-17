#!/bin/bash
#SBATCH --job-name=partb3
#SBATCH --time=00:30:00
#SBATCH --cpus-per-task=8
#SBATCH --output=partb3-%j.out

set -e
gcc -O2 -fopenmp -I. -o partb_3 src/partb_3.c
./partb_3 8

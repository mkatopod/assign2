#!/bin/bash
#SBATCH --job-name=partb1
#SBATCH --time=00:30:00
#SBATCH --output=partb1-%j.out

set -e
gcc -O2 -fopenmp -I. -o partb_1 src/partb_1.c
./partb_1

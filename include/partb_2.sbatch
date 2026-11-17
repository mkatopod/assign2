#!/bin/bash
#SBATCH --job-name=partb2
#SBATCH --time=00:30:00
#SBATCH --cpus-per-task=8
#SBATCH --output=partb2-%j.out

set -e
gcc -O2 -fopenmp -I. -o partb_2 src/partb_2.c
./partb_2 8

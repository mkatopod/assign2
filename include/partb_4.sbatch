#!/bin/bash
#SBATCH --job-name=partb4
#SBATCH --time=00:30:00
#SBATCH --output=partb4-%j.out

set -e
gcc -O2 -I. -o partb_4 src/partb_4.c
./partb_4
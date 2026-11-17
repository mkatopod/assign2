#!/bin/bash
#SBATCH --job-name=parta1
#SBATCH --time=00:30:00
#SBATCH --output=parta1-%j.out

set -e
gcc -O2 -I. -o parta_1 src/parta_1.c
echo "Cache sizes on compute node:"
lscpu -C
./parta_1

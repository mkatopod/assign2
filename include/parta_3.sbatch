#!/bin/bash
#SBATCH --job-name=parta3
#SBATCH --time=00:30:00
#SBATCH --output=parta3-%j.out

set -e
gcc -O2 -o parta_3 src/parta_3.c
./parta_3
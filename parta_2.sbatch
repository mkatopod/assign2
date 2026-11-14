#!/bin/bash
#SBATCH --job-name=parta1
#SBATCH --time=00:30:00
#SBATCH --output=parta1-%j.out

set -e
gcc -O2 -o parta_2 parta_2.c
./parta_1
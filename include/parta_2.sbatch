#!/bin/bash
#SBATCH --job-name=parta2
#SBATCH --time=00:30:00
#SBATCH --output=parta2-%j.out

set -e
gcc -O2 -I. -o parta_2 src/parta_2.c
./parta_2

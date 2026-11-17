#!/bin/bash
#SBATCH --job-name=parta4
#SBATCH --time=00:30:00
#SBATCH --output=parta4-%j.out

set -e
gcc -O2 -I. -o parta_4 src/parta_4.c
<<<<<<< HEAD
./parta_4
=======
./parta_4
>>>>>>> 4ec3d62d6b826796585775e527b9af34d0672f3e

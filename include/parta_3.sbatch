#!/bin/bash
#SBATCH --job-name=parta3
#SBATCH --time=00:30:00
#SBATCH --output=parta3-%j.out

set -e
gcc -O2 -I. -o parta_3 src/parta_3.c
<<<<<<< HEAD
./parta_3
=======
./parta_3
>>>>>>> 4ec3d62d6b826796585775e527b9af34d0672f3e

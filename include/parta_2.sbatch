#!/bin/bash
#SBATCH --job-name=parta2
#SBATCH --time=00:30:00
#SBATCH --output=parta2-%j.out

set -e
gcc -O2 -I. -o parta_2 src/parta_2.c
<<<<<<< HEAD
./parta_2
=======
./parta_2
>>>>>>> 4ec3d62d6b826796585775e527b9af34d0672f3e

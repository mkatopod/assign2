#!/bin/bash
#SBATCH --job-name=parta1
#SBATCH --output=parta1-%j.out

gcc -O2 -o parta_1 parta_1.c
./parta_1

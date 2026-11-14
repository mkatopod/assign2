/*
    Demonstrates proper OpenMP timing.

    Compile:
    gcc -O2 -fopenmp partb_3.c -o partb_3

    Run:
    ./partb_3 8
*/

#include "assign.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void matmul_omp(
    double *A,
    double *B,
    double *C,
    int M,
    int K,
    int N) {
    #pragma omp parallel for \
        default(shared) \
        schedule(static)
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;

            for (int k = 0; k < K; k++) {
                sum +=
                    A[i * K + k] *
                    B[k * N + j];
            }

            C[i * N + j] = sum;
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,
            "Usage: %s <threads>\n",
            argv[0]);
        return 1;
    }

    int p = atoi(argv[1]);
    int M = 1024;
    int K = 1024;
    int N = 1024;

    double *A = generate_matrix(M, K, 1);
    double *B = generate_matrix(K, N, 2);
    double *C = calloc((size_t)M * N, sizeof(double));

    if (C == NULL) {
        fprintf(stderr,
            "Allocation failed\n");
        return 1;
    }

    /*
        Requirement:
        thread count set BEFORE
        timed region
    */
    omp_set_num_threads(p);
    /*
        Requirement:
        timer starts immediately
        before parallel work
    */
    double start = omp_get_wtime();

    matmul_omp(
        A,
        B,
        C,
        M,
        K,
        N);

    /*
        Requirement:
        timer stops immediately
        after parallel work
    */
    double elapsed = omp_get_wtime() - start;

    double rate =
        (2.0 * M * K * N) /
        elapsed /
        1.0e9;

    printf("Threads : %d\n", p);
    printf("Time    : %.4f seconds\n",
           elapsed);
    printf("Rate    : %.3f GFLOP/s\n",
           rate);

    free(A);
    free(B);
    free(C);

    return 0;
}
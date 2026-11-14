/*
    OpenMP One-Dimensional Partition

    Compile:
    gcc -O2 -fopenmp partb_1.c -o partb_1
*/

#include "assign.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void matmul_omp(
    double *A,
    double *B,
    double *C,
    int M,
    int K,
    int N,
    int b) {
    memset(C, 0, (size_t)M * N * sizeof(double));

    /* Untiled version */
    if (b == 0) {
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

    /* Tiled version */
    else {
        #pragma omp parallel for \
            default(shared) \
            schedule(static)

        for (int ii = 0; ii < M; ii += b) {
            for (int jj = 0; jj < N; jj += b) {
                for (int kk = 0; kk < K; kk += b) {
                    int i_end =
                        (ii + b < M)
                            ? ii + b
                            : M;

                    int j_end =
                        (jj + b < N)
                            ? jj + b
                            : N;

                    int k_end =
                        (kk + b < K)
                            ? kk + b
                            : K;

                    for (int i = ii;
                         i < i_end;
                         i++) {
                        for (int j = jj;
                             j < j_end;
                             j++) {
                            double sum =
                                C[i * N + j];

                            for (int k = kk;
                                 k < k_end;
                                 k++) {
                                sum +=
                                    A[i * K + k] *
                                    B[k * N + j];
                            }
                            C[i * N + j] = sum;
                        }
                    }
                }
            }
        }
    }
}


void print_thread_info(void) {
    #pragma omp parallel default(shared) {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        if (tid == 0) {
            printf("Running with %d threads\n",
                   nthreads);
        }
    }
}


/* This is a Run Test */
double run_case(
    int M,
    int K,
    int N,
    int p,
    int b) {
    double *A = generate_matrix(M, K, 1);
    double *B = generate_matrix(K, N, 2);
    double *C = calloc((size_t)M * N, sizeof(double));

    omp_set_num_threads(p);

    double start = omp_get_wtime();

    matmul_omp(
        A,
        B,
        C,
        M,
        K,
        N,
        b);

    double elapsed = omp_get_wtime() - start;

    double rate =
        (2.0 * M * K * N) /
        elapsed /
        1e9;

    printf(
        "Size=%d  p=%d  b=%d  "
        "Time=%8.4f  "
        "Rate=%8.3f GFLOP/s\n",
        M,
        p,
        b,
        elapsed,
        rate);

    free(A);
    free(B);
    free(C);

    return elapsed;
}


int main(void) {
    /*
       Replace with the actual b*
       from Part A.1, this is temporary for now
    */
    int b_star = 64;
    int pvals[] = { 1, 2, 4, 8, 12, 18, 36 };

    print_thread_info();

    printf("\n===== 1024 : b = 0 =====\n");

    for (int i = 0; i < 7; i++) {
        run_case(
            1024,
            1024,
            1024,
            pvals[i],
            0);
    }

    printf("\n===== 1024 : b = b* =====\n");

    for (int i = 0; i < 7; i++) {
        run_case(
            1024,
            1024,
            1024,
            pvals[i],
            b_star);
    }

    printf("\n===== 2048 : b = 0 =====\n");

    for (int i = 0; i < 7; i++) {
        run_case(
            2048,
            2048,
            2048,
            pvals[i],
            0);
    }

    printf("\n===== 2048 : b = b* =====\n");

    for (int i = 0; i < 7; i++) {
        run_case(
            2048,
            2048,
            2048,
            pvals[i],
            b_star);
    }

    return 0;
}
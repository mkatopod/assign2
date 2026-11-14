#include "assign.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void matmul_seq(
    double *A,
    double *B,
    double *C,
    int M,
    int K,
    int N) {
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

/* ---------------------------------------------------------
   Tiled multiplication (b > 0)
--------------------------------------------------------- */
void matmul_tiled(
    double *A,
    double *B,
    double *C,
    int M,
    int K,
    int N,
    int b) {
    memset(C, 0, (size_t)M * N * sizeof(double));

    for (int ii = 0; ii < M; ii += b) {
        for (int jj = 0; jj < N; jj += b) {
            for (int kk = 0; kk < K; kk += b) {
                int i_end = (ii + b < M) ? ii + b : M;
                int j_end = (jj + b < N) ? jj + b : N;
                int k_end = (kk + b < K) ? kk + b : K;

                for (int i = ii; i < i_end; i++) {
                    for (int j = jj; j < j_end; j++) {
                        double sum = C[i * N + j];

                        for (int k = kk; k < k_end; k++) {
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


void run_case(
    int M,
    int K,
    int N,
    int b) {
    double *A = generate_matrix(M, K, 1);
    double *B = generate_matrix(K, N, 2);
    double *C = (double *)malloc( (size_t)M * N * sizeof(double));

    if (C == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    double start = now_seconds();

    if (b == 0) {
        matmul_seq(A, B, C, M, K, N);
    }
    else {
        matmul_tiled(A, B, C, M, K, N, b);
    }

    double elapsed = now_seconds() - start;
    double flops = 2.0 * M * K * N;
    double gflops = flops / elapsed / 1.0e9;

    printf("%5d %5d %10.4f %10.3f\n",
           M,
           b,
           elapsed,
           gflops);

    free(A);
    free(B);
    free(C);
}


int main(void) {
    printf("\n");
    printf(" Matrix Size  Tile      Time(s)    GFLOP/s\n");
    printf("-------------------------------------------\n");

    run_case(1024, 1024, 1024, 0);
    run_case(2048, 2048, 2048, 0);
    run_case(2048, 2048, 2048, 16);
    run_case(2048, 2048, 2048, 32);
    run_case(2048, 2048, 2048, 64);
    run_case(2048, 2048, 2048, 128);
    run_case(2048, 2048, 2048, 256);

    return 0;
}
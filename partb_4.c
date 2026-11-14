/*
    partb_4.c

    OpenMP partition implementation

    gcc -O2 -fopenmp partb_4.c -o partb_4
*/

#include "assign.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void matmul_2d_partition(
    double *A,
    double *B,
    double *C,
    int M,
    int K,
    int N,
    int pr,
    int pc,
    int b) {
    int p = pr * pc;

    memset(C, 0, (size_t)M * N * sizeof(double));

    #pragma omp parallel for schedule(static) default(shared)
    for (int t = 0; t < p; t++) {
        int r = t / pc;
        int c = t % pc;

        int row_start = (r * M) / pr;
        int row_end = ((r + 1) * M) / pr;
        int col_start = (c * N) / pc;
        int col_end = ((c + 1) * N) / pc;

        for (int ii = row_start;
             ii < row_end;
             ii += b) {
            for (int jj = col_start;
                 jj < col_end;
                 jj += b) {
                for (int kk = 0;
                     kk < K;
                     kk += b) {
                    int i_end =
                        (ii + b < row_end)
                        ? ii + b
                        : row_end;

                    int j_end =
                        (jj + b < col_end)
                        ? jj + b
                        : col_end;

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,
                "Usage: %s <threads>\n",
                argv[0]);
        return 1;
    }

    int p = atoi(argv[1]);

    omp_set_num_threads(p);

    int b_star = 64;
    int M = 2048;
    int K = 2048;
    int N = 2048;
    int pr = 6;
    int pc = 6;

    double *A = generate_matrix(M, K, 1);
    double *B = generate_matrix(K, N, 2);
    double *C = calloc((size_t)M * N, sizeof(double));

    double start = omp_get_wtime();

    matmul_2d_partition(
        A,
        B,
        C,
        M,
        K,
        N,
        pr,
        pc,
        b_star);

    double elapsed = omp_get_wtime() - start;

    printf("Time = %.4f seconds\n",
           elapsed);

    free(A);
    free(B);
    free(C);

    return 0;
}
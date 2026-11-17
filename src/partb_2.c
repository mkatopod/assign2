#include "assignment2.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*matmul_omp is the OpenMP parallel matric multiplciation function*/
void matmul_omp(
    double *A,
    double *B,
    double *C,
    int M,
    int K,
    int N,
    int b) {
    memset(C, 0, (size_t)M * N * sizeof(double));

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

                    for (int i = ii; i < i_end; i++) {
                        for (int j = jj; j < j_end; j++) {
                            double sum =
                                C[i * N + j];

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
}

/* run_case is a Run Test */
void run_case(
    int M,
    int K,
    int N,
    int p,
    int b) {
    double *A = generate_matrix(M, K, 1);
    double *B = generate_matrix(K, N, 2);
    double *C = calloc((size_t)M * N, sizeof(double));

    if (C == NULL) {
        fprintf(stderr,
                "Allocation failed\n");
        exit(1);
    }

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
        1.0e9;

    printf(
        "M=%d p=%d b=%d "
        "Time=%8.4f "
        "Rate=%8.3f GFLOP/s\n",
        M,
        p,
        b,
        elapsed,
        rate);

    free(A);
    free(B);
    free(C);
}

/*In main, we print the results*/
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,
                "Usage: %s <threads>\n",
                argv[0]);
        return 1;
    }

    int p = atoi(argv[1]);

    int b_star = 64;

    printf("\n");
    printf("OpenMP Matrix Multiplication\n");
    printf("Threads = %d\n\n", p);

    /*For: 1024, b = 0 */
    run_case(
        1024,
        1024,
        1024,
        p,
        0);

    /*For: 1024, b = b* */
    run_case(
        1024,
        1024,
        1024,
        p,
        b_star);

    /*For: 2048, b = 0 */
    run_case(
        2048,
        2048,
        2048,
        p,
        0);

    /*For: 2048, b = b* */
    run_case(
        2048,
        2048,
        2048,
        p,
        b_star);

    return 0;
}
/*
    Part A.4
    Non-divisible Dimensions

    Compile:
    gcc -O2 -pthread part4_nondivisible.c -o part4_nondivisible
*/

#include "assignment2.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    double *A;
    double *B;
    double *C;
    int M;
    int K;
    int N;
    int row_start;
    int row_end;
    int col_start;
    int col_end;
    int b;
} thread_arg_t;


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
                sum += A[i * K + k] *
                       B[k * N + j];
            }

            C[i * N + j] = sum;
        }
    }
}


void *worker(void *arg) {
    thread_arg_t *t = (thread_arg_t *)arg;
    int b = t->b;

    for (int ii = t->row_start;
         ii < t->row_end;
         ii += b) {
        for (int jj = t->col_start;
             jj < t->col_end;
             jj += b) {
            for (int kk = 0;
                 kk < t->K;
                 kk += b) {
                int i_end =
                    (ii + b < t->row_end)
                        ? ii + b
                        : t->row_end;

                int j_end =
                    (jj + b < t->col_end)
                        ? jj + b
                        : t->col_end;

                int k_end =
                    (kk + b < t->K)
                        ? kk + b
                        : t->K;

                for (int i = ii;
                     i < i_end;
                     i++) {
                    for (int j = jj;
                         j < j_end;
                         j++) {
                        double sum =
                            t->C[i * t->N + j];

                        for (int k = kk;
                             k < k_end;
                             k++) {
                            sum +=
                                t->A[i * t->K + k] *
                                t->B[k * t->N + j];
                        }

                        t->C[i * t->N + j] = sum;
                    }
                }
            }
        }
    }
    return NULL;
}


int verify(
    double *Cseq,
    double *Cpar,
    int M,
    int N) {
    size_t count = (size_t)M * N;

    for (size_t i = 0; i < count; i++) {
        if (Cseq[i] != Cpar[i]) {
            return 0;
        }
    }

    return 1;
}


int main(void) {
    /*
       Replace with your actual b*
       from Part A.1, this is temporary
    */
    int b_star = 64;

    int M = 1000;
    int K = 1024;
    int N = 1536;

    int pr = 6;
    int pc = 6;
    int p = 36;

    double *A = generate_matrix(M, K, 1);
    double *B = generate_matrix(K, N, 2);

    double *Cseq = calloc((size_t)M * N, sizeof(double));
    double *Cpar = calloc((size_t)M * N, sizeof(double));

    if (Cseq == NULL || Cpar == NULL) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }

    /* Sequential reference result */
    matmul_seq(
        A,
        B,
        Cseq,
        M,
        K,
        N);

    pthread_t threads[36];
    thread_arg_t args[36];

    double start = now_seconds();
    int tid = 0;

    for (int r = 0; r < pr; r++) {
        for (int c = 0; c < pc; c++) {
            args[tid].A = A;
            args[tid].B = B;
            args[tid].C = Cpar;

            args[tid].M = M;
            args[tid].K = K;
            args[tid].N = N;
            args[tid].b = b_star;

            args[tid].row_start = (r * M) / pr;
            args[tid].row_end = ((r + 1) * M) / pr;
            args[tid].col_start = (c * N) / pc;
            args[tid].col_end = ((c + 1) * N) / pc;

            pthread_create(
                &threads[tid],
                NULL,
                worker,
                &args[tid]);

            tid++;
        }
    }

    for (int t = 0; t < p; t++) {
        pthread_join(
            threads[t],
            NULL);
    }

    double elapsed = now_seconds() - start;

    double rate =
        (2.0 * M * K * N) /
        elapsed /
        1.0e9;

    printf("\n");
    printf("M = %d\n", M);
    printf("K = %d\n", K);
    printf("N = %d\n", N);
    printf("Grid = %d x %d\n", pr, pc);
    printf("b* = %d\n", b_star);
    printf("Time = %.4f seconds\n", elapsed);
    printf("Rate = %.3f GFLOP/s\n", rate);

    if (verify(Cseq, Cpar, M, N)) {
        printf("CORRECT: Parallel result matches sequential result.\n");
    }
    else { 
        printf("ERROR: Results do not match.\n");
    }

    free(A);
    free(B);
    free(Cseq);
    free(Cpar);
    return 0;
}
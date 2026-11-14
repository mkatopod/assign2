/*
    Two-Dimensional Partition
    Compile:

    gcc -O2 -pthread parta_3.c -o parta_3
*/

#include "assign.h"
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


void *worker_2d(void *arg) {
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


int find_pr(int p) {
    int pr = 1;
    for (int d = 1; d * d <= p; d++) {
        if (p % d == 0) {
            pr = d;
        }
    }
    return pr;
}


double run_2d(
    int M,
    int K,
    int N,
    int p,
    int pr,
    int pc,
    int b) {
    double *A = generate_matrix(M, K, 1);
    double *B = generate_matrix(K, N, 2);
    double *C = calloc((size_t)M * N, sizeof(double));

    pthread_t *threads = malloc(p * sizeof(pthread_t));
    thread_arg_t *args = malloc(p * sizeof(thread_arg_t));
    double start =now_seconds();

    int tid = 0;

    for (int r = 0; r < pr; r++) {
        for (int c = 0; c < pc; c++) {
            args[tid].A = A;
            args[tid].B = B;
            args[tid].C = C;

            args[tid].M = M;
            args[tid].K = K;
            args[tid].N = N;

            args[tid].b = b;
            args[tid].row_start = (r * M) / pr;
            args[tid].row_end = ((r + 1) * M) / pr;

            args[tid].col_start = (c * N) / pc;
            args[tid].col_end = ((c + 1) * N) / pc;

            pthread_create(
                &threads[tid],
                NULL,
                worker_2d,
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
        1e9;

    printf(
        "M=%d p=%d grid=%dx%d "
        "time=%8.4f "
        "rate=%8.3f GFLOP/s\n",
        M,
        p,
        pr,
        pc,
        elapsed,
        rate);

    free(A);
    free(B);
    free(C);
    free(threads);
    free(args);

    return elapsed;
}


int main() {
    /* Replace with result found from Part A.1, this is temporary below */
    int b_star = 64;

    int plist[] = {2,4,8,12,18,36};

    printf("\n===== SIZE 1024 =====\n");

    for(int i=0;i<6;i++) {
        int p = plist[i];
        int pr = find_pr(p);
        int pc = p / pr;

        run_2d(1024, 1024, 1024, p, pr, pc, b_star);
    }

    printf("\n===== SIZE 2048 =====\n");

    for(int i=0;i<6;i++) {
        int p = plist[i];
        int pr = find_pr(p);
        int pc = p / pr;

        run_2d(2048, 2048, 2048, p, pr, pc, b_star);
    }

    /*
       Special case required:

       p = 36
       grid = 1 x 36
       size = 2048
    */

    printf("\n===== 2048 COLUMN GRID 1x36 =====\n");
    run_2d(2048, 2048, 2048, 36, 1, 36, b_star);

    return 0;
}
#include "assign.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------*/
/* Thread argument structure                                */
/*----------------------------------------------------------*/

typedef struct {
    double *A;
    double *B;
    double *C;
    int M;
    int K;
    int N;
    int start_row;
    int end_row;
    int b;
} thread_arg_t;

/*----------------------------------------------------------*/
/* Worker thread                                            */
/*----------------------------------------------------------*/

void *worker_1d(void *arg) {
    thread_arg_t *t = (thread_arg_t *)arg;

    if (t->b == 0) {
        for (int i = t->start_row; i < t->end_row; i++) {
            for (int j = 0; j < t->N; j++) {
                double sum = 0.0;

                for (int k = 0; k < t->K; k++) {
                    sum +=
                        t->A[i * t->K + k] *
                        t->B[k * t->N + j];
                }
                t->C[i * t->N + j] = sum;
            }
        }
    }


    else {
        int b = t->b;

        for (int ii = t->start_row; ii < t->end_row; ii += b) {
            for (int jj = 0; jj < t->N; jj += b) {
                for (int kk = 0; kk < t->K; kk += b) {
                    int i_end =
                        (ii + b < t->end_row)
                        ? ii + b
                        : t->end_row;

                    int j_end =
                        (jj + b < t->N)
                        ? jj + b
                        : t->N;

                    int k_end =
                        (kk + b < t->K)
                        ? kk + b
                        : t->K;

                    for (int i = ii; i < i_end; i++) {
                        for (int j = jj; j < j_end; j++) {
                            double sum =
                                t->C[i * t->N + j];

                            for (int k = kk; k < k_end; k++) {
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
    }
    return NULL;
}

/*----------------------------------------------------------*/
/* Parallel execution                                       */
/*----------------------------------------------------------*/

double run_parallel(
    int M,
    int K,
    int N,
    int p,
    int b) {
    double *A =
        generate_matrix(M, K, 1);

    double *B =
        generate_matrix(K, N, 2);

    double *C =
        (double *)calloc(
            (size_t)M * N,
            sizeof(double));

    if (!C) {
        fprintf(stderr, "Allocation failed\n");
        exit(1);
    }

    pthread_t *threads =
        malloc(p * sizeof(pthread_t));

    thread_arg_t *args =
        malloc(p * sizeof(thread_arg_t));

    double start = now_seconds();

    for (int t = 0; t < p; t++) {
        args[t].A = A;
        args[t].B = B;
        args[t].C = C;

        args[t].M = M;
        args[t].K = K;
        args[t].N = N;

        args[t].b = b;
        args[t].start_row =
            (t * M) / p;
        args[t].end_row =
            ((t + 1) * M) / p;

        pthread_create(
            &threads[t],
            NULL,
            worker_1d,
            &args[t]);
    }

    for (int t = 0; t < p; t++) {
        pthread_join(
            threads[t],
            NULL);
    }

    double elapsed =
        now_seconds() - start;

    double gflops =
        (2.0 * M * K * N) /
        elapsed /
        1.0e9;

    printf(
        "%5d %5d %5d %10.4f %10.3f\n",
        M,
        p,
        b,
        elapsed,
        gflops);

    free(A);
    free(B);
    free(C);
    free(threads);
    free(args);

    return elapsed;
}

/*----------------------------------------------------------*/
/* Main                                                     */
/*----------------------------------------------------------*/

int main(void) {
    /*this b_star is for testing temporary*/
    int b_star = 64;

    int pvals[] = {
        1, 2, 4, 8, 12, 18, 36
    };

    printf("\n");
    printf("Size     p     b      Time(s)    GFLOP/s\n");
    printf("-----------------------------------------\n");

    /*======================*/
    /* 1024, b = 0          */
    /*======================*/

    double Tseq1024 =
        run_parallel(
            1024,1024,1024,
            1,
            0);

    for(int i=1;i<7;i++) {
        double Tp =
            run_parallel(
                1024,1024,1024,
                pvals[i],
                0);

        printf("Speedup = %.3f\n",
               Tseq1024 / Tp);
    }

    /*======================*/
    /* 1024, b = b*         */
    /*======================*/

    double Tseq1024Tile =
        run_parallel(
            1024,1024,1024,
            1,
            b_star);

    for(int i=1;i<7;i++) {
        double Tp =
            run_parallel(
                1024,1024,1024,
                pvals[i],
                b_star);

        printf("Speedup = %.3f\n",
               Tseq1024Tile / Tp);
    }

    /*======================*/
    /* 2048, b = 0          */
    /*======================*/

    double Tseq2048 =
        run_parallel(
            2048,2048,2048,
            1,
            0);

    for(int i=1;i<7;i++) {
        double Tp =
            run_parallel(
                2048,2048,2048,
                pvals[i],
                0);

        printf("Speedup = %.3f\n",
               Tseq2048 / Tp);
    }

    /*======================*/
    /* 2048, b = b*         */
    /*======================*/

    double Tseq2048Tile =
        run_parallel(
            2048,2048,2048,
            1,
            b_star);

    for(int i=1;i<7;i++) {
        double Tp =
            run_parallel(
                2048,2048,2048,
                pvals[i],
                b_star);

        printf("Speedup = %.3f\n",
               Tseq2048Tile / Tp);
    }
    return 0;
}
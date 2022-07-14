#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E 9
/* TASK_SIZE: 10^(E-1) */
#define TASK_SIZE pow(10, E - 1)

unsigned int rand_interval(unsigned int min, unsigned int max) {
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    do {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}

void fillupRandomly(int *m, int size, unsigned int min, unsigned int max) {
    for (int i = 0; i < size; i++) m[i] = rand_interval(min, max);
}

void mergeSortAux(int *X, int n, int *tmp) {
    int i = 0;
    int j = n / 2;
    int ti = 0;

    while (i < n / 2 && j < n) {
        if (X[i] < X[j]) {
            tmp[ti] = X[i];
            ti++;
            i++;
        } else {
            tmp[ti] = X[j];
            ti++;
            j++;
        }
    }
    while (i < n / 2) {
        tmp[ti] = X[i];
        ti++;
        i++;
    }
    while (j < n) {
        tmp[ti] = X[j];
        ti++;
        j++;
    }

    memcpy(X, tmp, n * sizeof(int));
}

void mergeSort(int *X, int n, int *tmp) {
    if (n < 2) return;

#pragma omp task shared(X) if (n > TASK_SIZE)
    mergeSort(X, n / 2, tmp);

#pragma omp task shared(X) if (n > TASK_SIZE)
    mergeSort(X + (n / 2), n - (n / 2), tmp + n / 2);

#pragma omp taskwait
    mergeSortAux(X, n, tmp);
}

void init(int *a, int size) {
    for (int i = 0; i < size; i++) a[i] = 0;
}

int isSorted(int *a, int size) {
    for (int i = 0; i < size - 1; i++)
        if (a[i] > a[i + 1]) return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    srand(123456);

    int rep = 10;    /* 繰り返し回数 */
    double tmps = 0; /* 計算時間格納 */

    int N = (argc > 1) ? atoi(argv[1]) : pow(10, E); /* 要素数 */
    fprintf(stdout, "array size = %d\n", N);

    int numThreads = (argc > 2) ? atoi(argv[2]) : 2;

    int *X = malloc(N * sizeof(int));
    int *tmp = malloc(N * sizeof(int));

    omp_set_dynamic(0);
    omp_set_num_threads(numThreads);

    if (!X || !tmp) {
        if (X) free(X);
        if (tmp) free(tmp);
        return (EXIT_FAILURE);
    }

    fillupRandomly(X, N, 0, 5);

    /* 平均 */
    for (int i = 0; i < rep; i++) {
        double begin = omp_get_wtime();
#pragma omp parallel
        {
#pragma omp single
            mergeSort(X, N, tmp);
        }
        double end = omp_get_wtime();
        tmps += (end - begin);
    }
    fprintf(stdout, "elapsed time = %.5f [sec]\n", tmps / rep);

    /* 1回 */
    //     double begin = omp_get_wtime();
    // #pragma omp parallel
    //     {
    // #pragma omp single
    //         mergeSort(X, N, tmp);
    //     }
    //     double end = omp_get_wtime();
    // fprintf(stdout, "elapsed time = %.5f [sec]\n", end - begin);

    /* ソートされているか確認 */
    assert(1 == isSorted(X, N));

    free(X);
    free(tmp);
    return (EXIT_SUCCESS);
}

/*
merge_sort: O(nlogn)
gcc-11 -fopenmp kadai13-2.c -o kadai13-2
M1Mac

[Normal]
array size = 1000000
elapsed time = 0.08010 [sec]
19931568 [order]

array size = 10000000
elapsed time = 0.88970 [sec]
232534966 [order]

array size = 100000000
elapsed time = 10.48790 [sec]
2657542475 [order]

array size = 1000000000
elapsed time = 115.93267 [sec]
29897352854 [order]

[openMP]
array size = 1000000
elapsed time = 0.04300 [sec]

array size = 10000000
elapsed time = 0.45841 [sec]

array size = 100000000
elapsed time = 4.91482 [sec]

array size = 1000000000
elapsed time = 65.85070 [sec]
*/

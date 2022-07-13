#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merge(int a[], int b[], int left, int center, int right, int n) {
  int i, j, k;

  k = i = left;
  j = center + 1;
  while (i <= center && j <= right) {
    if (a[i] <= a[j])
      b[k++] = a[i++];
    else
      b[k++] = a[j++];
  }
  while (i <= center)
    b[k++] = a[i++];
  while (j <= right)
    b[k++] = a[j++];
  for (i = left; i <= right; i++)
    a[i] = b[i];
}

void merge_sort(int a[], int b[], int left, int right, int n) {
  if (left < right) {
    int center = (left + right) / 2;
    merge_sort(a, b, left, center, n);
    merge_sort(a, b, center + 1, right, n);
    merge(a, b, left, center, right, n);
  }
}

int main(int argc, char *argv[]) {
  int i, n = pow(10, 6);
  int rep = 10;
  double tmp = 0;
  int *x, *w;
  clock_t start, stop;

  if (argc > 1)
    n = atoi(argv[1]);
  fprintf(stdout, "array size = %d\n", n);

  x = (int *)calloc(n, sizeof(int));
  w = (int *)calloc(n, sizeof(int));
  srand((unsigned)time(NULL));

  for (i = 0; i < n; i++)
    x[i] = rand();

  /* 平均 */
  for (i = 0; i < rep; i++) {
    start = clock();
    merge_sort(x, w, 0, n - 1, n);
    stop = clock();
    tmp += (double)(stop - start) / (CLOCKS_PER_SEC);
  }

  /* 1回 */
  // start = clock();
  // merge_sort(x, w, 0, n - 1, n);
  // stop = clock();

  for (i = 0; i < n - 1; i++)
    if (x[i] > x[i + 1])
      printf("error!! [%d]\n", i);

  /* 平均 */
  fprintf(stdout, "elapsed time = %.5f [sec]\n", tmp / rep);

  /* 1回 */
  // fprintf(stdout, "elapsed time = %.5f [sec]\n",
  //         (double)(stop - start) / (CLOCKS_PER_SEC));

  free(x);
  free(w);

  return 0;
}

/*
merge_sort: O(nlogn)

M1Mac
array size = 1024
elapsed time = 0.00013 [sec]
10240 [order]

array size = 10000
elapsed time = 0.00163 [sec]
132877 [order]

array size = 100000
elapsed time = 0.00939 [sec]
1660964 [order]

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
*/

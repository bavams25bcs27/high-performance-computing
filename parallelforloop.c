
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000000  // size of large arrays

int main() {
    int *A, *B, *C;

    // Allocate memory
    A = (int *)malloc(N * sizeof(int));
    B = (int *)malloc(N * sizeof(int));
    C = (int *)malloc(N * sizeof(int));

    // Initialize arrays
    for (int i = 0; i < N; i++) {
        A[i] = rand() % 1000;
        B[i] = rand() % 1000;
    }

    // Parallel computation
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        if (A[i] > B[i])
            C[i] = A[i];
        else
            C[i] = B[i];
    }

    // Print sample output
    printf("Sample results:\n");
    for (int i = 0; i < 10; i++) {
        printf("A[%d]=%d, B[%d]=%d, Max=%d\n",
               i, A[i], i, B[i], C[i]);
    }

    // Free memory
    free(A);
    free(B);
    free(C);

    return 0;
}

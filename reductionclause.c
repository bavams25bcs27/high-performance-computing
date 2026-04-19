#include <stdio.h>
#include <omp.h>
#include <math.h>

// Function to check if a number is prime
int is_prime(int n) {
    if (n <= 1)
        return 0;

    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

int main() {
    int N, count = 0;

    printf("Enter value of N: ");
    scanf("%d", &N);

    // Parallel loop with reduction
    #pragma omp parallel for reduction(+:count)
    for (int i = 1; i <= N; i++) {
        if (is_prime(i)) {
            count += 1;
        }
    }

    printf("Total number of primes between 1 and %d = %d\n", N, count);

    return 0;
}

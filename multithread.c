#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <math.h>

#define BUFFER_SIZE 10
#define MAX_NUM 100

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty, full;
pthread_mutex_t mutex;
pthread_mutex_t file_mutex;

FILE *fp;

// Function to check prime
int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

// Function to print factors
void find_factors(int n, char *result) {
    char temp[200] = "";
    for (int i = 1; i <= n; i++) {
        if (n % i == 0) {
            char buf[10];
            sprintf(buf, "%d ", i);
            strcat(temp, buf);
        }
    }
    strcpy(result, temp);
}

// Producer thread
void *producer(void *arg) {
    while (1) {
        int num = rand() % MAX_NUM + 1;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = num;
        printf("Produced: %d\n", num);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        sleep(1);
    }
}

// Worker thread
void *worker(void *arg) {
    while (1) {
        int num;

        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        num = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        // Processing
        int prime = is_prime(num);
        char factors[200];
        find_factors(num, factors);

        char type[10];
        if (num % 2 == 0)
            sprintf(type, "Even");
        else
            sprintf(type, "Odd");

        // Write to file
        pthread_mutex_lock(&file_mutex);

        fprintf(fp, "Number: %d | %s | %s | Factors: %s\n",
                num,
                prime ? "Prime" : "Not Prime",
                type,
                factors);

        fflush(fp);

        pthread_mutex_unlock(&file_mutex);
    }
}

int main() {
    pthread_t prod;
    pthread_t workers[3];

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&file_mutex, NULL);

    fp = fopen("results.txt", "w");

    // Create producer
    pthread_create(&prod, NULL, producer, NULL);

    // Create worker threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&workers[i], NULL, worker, NULL);
    }

    // Join threads (runs forever)
    pthread_join(prod, NULL);
    for (int i = 0; i < 3; i++) {
        pthread_join(workers[i], NULL);
    }

    fclose(fp);
    return 0;
}


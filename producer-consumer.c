#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_QUEUE 100
#define MAX_LINE 256
#define NUM_CONSUMERS 2

// Queue structure
char queue[MAX_QUEUE][MAX_LINE];
int front = 0, rear = 0, count = 0;

omp_lock_t lock;
int done = 0;  // flag to indicate producers finished

// Enqueue
void enqueue(char *line) {
    while (1) {
        omp_set_lock(&lock);

        if (count < MAX_QUEUE) {
            strcpy(queue[rear], line);
            rear = (rear + 1) % MAX_QUEUE;
            count++;

            omp_unset_lock(&lock);
            break;
        }

        omp_unset_lock(&lock);
    }
}

// Dequeue
int dequeue(char *line) {
    int success = 0;

    omp_set_lock(&lock);

    if (count > 0) {
        strcpy(line, queue[front]);
        front = (front + 1) % MAX_QUEUE;
        count--;
        success = 1;
    }

    omp_unset_lock(&lock);
    return success;
}

// Producer: read from file
void producer(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), fp)) {
        enqueue(line);
    }

    fclose(fp);
}

// Consumer: write to output file
void consumer(FILE *out) {
    char line[MAX_LINE];

    while (1) {
        if (dequeue(line)) {
            #pragma omp critical
            {
                fputs(line, out);
            }
        } else if (done) {
            break;
        }
    }
}

int main() {
    char *files[] = {"file1.txt", "file2.txt", "file3.txt"};
    int num_files = 3;

    FILE *out = fopen("output.txt", "w");

    omp_init_lock(&lock);

    #pragma omp parallel sections
    {
        // Producer section
        #pragma omp section
        {
            for (int i = 0; i < num_files; i++) {
                producer(files[i]);
            }
            done = 1;
        }

        // Consumer sections
        #pragma omp section
        {
            consumer(out);
        }

        #pragma omp section
        {
            consumer(out);
        }
    }

    omp_destroy_lock(&lock);
    fclose(out);

    printf("Merging completed. Check output.txt\n");

    return 0;
}

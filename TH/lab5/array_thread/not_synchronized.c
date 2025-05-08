#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 100

int a[MAX_SIZE];        // Mảng dùng chung
int size = 0;           // Kích thước hiện tại của mảng

void* producer(void* arg) {
    while (1) {
        int num = rand() % 100;

        if (size < MAX_SIZE) {
            a[size++] = num;
            printf("[Producer] Added %d to array. Size = %d\n", num, size);
        } else {
            printf("[Producer] Array full. Skipping...\n");
        }

        usleep(100);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        if (size > 0) {
            int index = rand() % size;
            int value = a[index];
            a[index] = a[size - 1];
            size--;
            printf("[Consumer] Removed %d from array. Size = %d\n", value, size);
        } else {
            printf("[Consumer] Nothing in array a\n");
        }

        usleep(100);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}

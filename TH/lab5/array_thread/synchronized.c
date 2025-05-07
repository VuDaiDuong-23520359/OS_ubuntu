#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_SIZE 100

int a[MAX_SIZE];        // Mảng dùng chung
int size = 0;           // Kích thước hiện tại của mảng

sem_t mutex;            // Semaphore để đồng bộ

void* producer(void* arg) {
    while (1) {
        int num = rand() % 100;

        sem_wait(&mutex);  // Lock mảng a

        if (size < MAX_SIZE) {
            a[size++] = num;
            printf("[Producer] Added %d to array. Size = %d\n", num, size);
        } else {
            printf("[Producer] Array full. Skipping...\n");
        }

        sem_post(&mutex);  // Unlock mảng a

        sleep(1);  // Đợi để dễ quan sát
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        sem_wait(&mutex);  // Lock mảng a

        if (size > 0) {
            int index = rand() % size;  // Lấy phần tử bất kỳ
            int value = a[index];
            a[index] = a[size - 1];     // Thay thế bằng phần tử cuối để xóa
            size--;
            printf("[Consumer] Removed %d from array. Size = %d\n", value, size);
        } else {
            printf("[Consumer] Nothing in array a\n");
        }

        sem_post(&mutex);  // Unlock mảng a

        sleep(2);  // Đợi lâu hơn producer
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    sem_init(&mutex, 0, 1);  // Khởi tạo semaphore với giá trị 1

    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    sem_destroy(&mutex);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_BUFFER 359

int products = 0;
int sells = 0;

sem_t sem_product;  // số lượng sản phẩm có thể bán
sem_t sem_space;    // số lượng chỗ còn trống để sản xuất

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* producer(void* arg) {
    while (1) {
        sem_wait(&sem_space); // chờ nếu đã đầy (products == sells + 359)

        pthread_mutex_lock(&lock);
        products++;
        int p = products, s = sells;
        pthread_mutex_unlock(&lock);

        printf("[Producer] Produced: %d, Sold: %d\n", p, s);

        sem_post(&sem_product); // báo cho seller là có hàng
        sleep(3);
    }
    return NULL;
}

void* seller(void* arg) {
    while (1) {
        sem_wait(&sem_product); // chờ nếu chưa có hàng (products == sells)

        pthread_mutex_lock(&lock);
        sells++;
        int p = products, s = sells;
        pthread_mutex_unlock(&lock);

        printf("[Seller]   Produced: %d, Sold: %d\n", p, s);

        sem_post(&sem_space); // báo cho producer là có thêm chỗ trống
        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Khởi tạo semaphore
    sem_init(&sem_product, 0, 0);         // Ban đầu chưa có sản phẩm
    sem_init(&sem_space, 0, MAX_BUFFER);  // Tối đa 359 sản phẩm (products - sells <= 359)

    pthread_create(&t1, NULL, producer, NULL);
    pthread_create(&t2, NULL, seller, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Giải phóng tài nguyên
    pthread_mutex_destroy(&lock);
    sem_destroy(&sem_product);
    sem_destroy(&sem_space);

    return 0;
}

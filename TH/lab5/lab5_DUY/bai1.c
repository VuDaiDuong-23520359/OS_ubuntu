#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_BUFFER 372 // MSSV 23520372

int products = 0;
int sells = 0;

sem_t sem_product;
sem_t sem_store;

void* producer(void* arg){
    while (1){
        sem_wait(&sem_store); // chờ bán nếu kho đầy (products == sells + 371)

        products++;
        printf("[Producer] Product: %d, Sell: %d\n", products, sells);

        sem_post(&sem_product); // báo cho seller là có hàng
        sleep(0);
    }
    return NULL;
}

void* seller(void* arg){
    while (1){
        sem_wait(&sem_product); // chờ nếu chưa có hàng (products == sells)

        sells++;
        printf("[Seller] Product: %d, Sell: %d\n", products, sells);

        sem_post(&sem_store); // báo cho producer là có thêm chỗ trống
        sleep(1);
    }
    return NULL;
}

int main(){
    pthread_t t1, t2;

    // Khởi tạo semaphore
    sem_init(&sem_product, 0, 0);         // Ban đầu chưa có sản phẩm
    sem_init(&sem_store, 0, MAX_BUFFER);  // Tối đa 372 sản phẩm (products - sells <= 371)

    pthread_create(&t1, NULL, producer, NULL);
    pthread_create(&t2, NULL, seller, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&sem_product);
    sem_destroy(&sem_store);

    return 0;
}

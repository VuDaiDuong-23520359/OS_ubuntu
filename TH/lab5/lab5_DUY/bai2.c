#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int* a;          // Mảng dùng chung
int n;           // Kích thước hiện tại của mảng

sem_t sem_create;
sem_t sem_consume;

void* create(void* arg){
    while (1){
        sem_wait(&sem_consume);
        int size = 0;
        while(size < n && a[size] != -1){
            size++;
        }
        int num = rand() % 100;
        a[size] = num;
        printf("[Create] Create: %d. Array size: %d\n", num, size + 1);
        sem_post(&sem_create);
    }
    return NULL;
}

void* consume(void* arg){
    while (1){
        sem_wait(&sem_create);
        int size = 0;
        while(size < n && a[size] != -1){
            size++;
        }
        if (size > 0){
            int index = size % 2;
            int value = a[index];
            a[index] = a[size - 1];
            a[size - 1] = -1;
            size--;
            printf("[Consume] Removed %d from array. Array size = %d\n", value, size);
        }
        else{
            printf("[Consume] Nothing in array a\n");
        }
        sem_post(&sem_consume);
    }
    return NULL;
}

int main(){
    srand(time(NULL));
    printf("Nhap n: ");
    scanf("%d", &n);
    a = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++){
        a[i] = -1;
    }
    
    sem_init(&sem_create, 0, n);
    sem_init(&sem_consume, 0, 0);
    pthread_t writer, reader;

    pthread_create(&writer, NULL, create, NULL);
    pthread_create(&reader, NULL, consume, NULL);

    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    return 0;
}

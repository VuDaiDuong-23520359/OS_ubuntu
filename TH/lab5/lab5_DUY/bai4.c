#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int x = 0;
pthread_mutex_t lock;

void* processA(void* arg){
    while (1){
        pthread_mutex_lock(&lock);

        int temp = x;
        usleep(123456);
        x = temp + 1;
        if (x == 20)
            x = 0;
        printf("A: x = %d\n", x);

        pthread_mutex_unlock(&lock);
        usleep(123456);
    }
    return NULL;
}

void* processB(void* arg){
    while (1){
        pthread_mutex_lock(&lock);

        int temp = x;
        usleep(123456);
        x = temp + 1;
        if (x == 20)
            x = 0;
        printf("B: x = %d\n", x);

        pthread_mutex_unlock(&lock);
        usleep(123456);

    }
    return NULL;
}

int main(){
    pthread_t PA, PB;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&PA, NULL, processA, NULL);
    pthread_create(&PB, NULL, processB, NULL);

    pthread_exit(NULL);
    return 0;
}

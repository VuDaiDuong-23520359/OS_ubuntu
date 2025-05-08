#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int x = 0;
pthread_mutex_t lock;

void* processA(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);

        x = x + 1;
        if (x == 20)
            x = 0;
        printf("A: x = %d\n", x);

        pthread_mutex_unlock(&lock);
        usleep(80);
    }
    return NULL;
}

void* processB(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);

        x = x + 1;
        if (x == 20)
            x = 0;
        printf("B: x = %d\n", x);

        pthread_mutex_unlock(&lock);
        usleep(100);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&tid1, NULL, processA, NULL);
    pthread_create(&tid2, NULL, processB, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_mutex_destroy(&lock);
    return 0;
}

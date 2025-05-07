#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int x = 0;

void* processA(void* arg) {
    while (1) {
        x = x + 1;
        if (x == 20)
            x = 0;
        printf("A: x = %d\n", x);
        sleep(2);
    }
    return NULL;
}

void* processB(void* arg) {
    while (1) {
        x = x + 1;
        if (x == 20)
            x = 0;
        printf("B: x = %d\n", x);
        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, processA, NULL);
    pthread_create(&tid2, NULL, processB, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}

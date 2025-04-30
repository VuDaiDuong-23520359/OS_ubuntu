#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>

#define N 100

typedef struct {
    int a[N];
    int size;
} SharedArray;

#define SHM_NAME "/shm_array"
#define SEM_NAME "/sem_array"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedArray));
    SharedArray *shared = mmap(NULL, sizeof(SharedArray), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);

    while (1) {
        sem_wait(sem);
        if (shared->size < N) {
            int val = rand() % 1000;
            shared->a[shared->size++] = val;
            printf("[Producer] Added %d → Size = %d\n", val, shared->size);
        }
        sem_post(sem);
        sleep(1);
    }

    return 0;
}

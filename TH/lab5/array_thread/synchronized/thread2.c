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
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    SharedArray *shared = mmap(NULL, sizeof(SharedArray), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem = sem_open(SEM_NAME, 0);

    while (1) {
        sem_wait(sem);
        if (shared->size > 0) {
            int val = shared->a[--shared->size];
            printf("[Consumer] Removed %d → Size = %d\n", val, shared->size);
        } else {
            printf("[Consumer] Nothing in array a\n");
        }
        sem_post(sem);
        sleep(2);
    }

    return 0;
}

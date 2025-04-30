// producer.c (version không đồng bộ)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define N 100

typedef struct {
    int a[N];
    int size;
} SharedArray;

#define SHM_NAME "/shm_array"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedArray));
    SharedArray *shared = mmap(NULL, sizeof(SharedArray), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (1) {
        if (shared->size < N) {
            int val = rand() % 1000;
            shared->a[shared->size++] = val;
            printf("[Producer] Added %d → Size = %d\n", val, shared->size);
        }
        sleep(1);
    }

    return 0;
}

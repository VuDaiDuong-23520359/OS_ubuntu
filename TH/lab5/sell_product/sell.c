// processA.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>

#define SHM_NAME "/shm_counter"
#define MAX_SELLS 100

int main() {
    // Mở shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    int *shared = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    sem_t *sem_full = sem_open("/sem_full", 0);
    sem_t *sem_space = sem_open("/sem_space", 0);

    if (sem_full == SEM_FAILED || sem_space == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    while (shared[1] < MAX_SELLS) {
        sem_wait(sem_full);
        shared[1]++; // sells++
        printf("Process A: Bán sản phẩm #%d\n", shared[0]);
        sem_post(sem_space);
        sleep(1);
    }

    munmap(shared, 2 * sizeof(int));
    close(shm_fd);
    sem_close(sem_full);
    sem_close(sem_space);

    // Gỡ shared memory & semaphore (nếu bạn muốn làm ở process A)
    shm_unlink(SHM_NAME);
    sem_unlink("/sem_full");
    sem_unlink("/sem_space");

    return 0;
}

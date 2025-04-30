// processB.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>

#define SHM_NAME "/shm_counter"
#define MAX_PRODUCTS 459
#define THRESHOLD 300
#define MAX_IDLE 10

int main() {
    // Tạo hoặc mở shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, 2 * sizeof(int));
    int *shared = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    shared[0] = 0; // products
    shared[1] = 0;   // sells

    sem_t *sem_full = sem_open("/sem_full", O_CREAT, 0644, 0);
    sem_t *sem_space = sem_open("/sem_space", O_CREAT, 0644, 359);

    if (sem_full == SEM_FAILED || sem_space == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    int no_sell_counter = 0;

    while (shared[0] < MAX_PRODUCTS) {
        sem_wait(sem_space);
        shared[0]++; // products++
        printf("Process B: Tạo sản phẩm #%d\n", shared[0]);
        sem_post(sem_full);

        // Kiểm tra tồn kho
        int sval;
        sem_getvalue(sem_full, &sval);
        if (sval > THRESHOLD) {
            no_sell_counter++;
            if (no_sell_counter >= MAX_IDLE) {
                printf("⚠ Tồn kho quá nhiều mà không bán → Dừng sản xuất.\n");
                break;
            }
        } else {
            no_sell_counter = 0; // reset nếu hàng bán lại
        }

        sleep(1);
    }

    munmap(shared, 2 * sizeof(int));
    close(shm_fd);
    sem_close(sem_full);
    sem_close(sem_space);

    return 0;
}

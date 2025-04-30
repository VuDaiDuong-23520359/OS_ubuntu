// processB
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SHM_NAME "/shm_x"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    int *x = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (1) {
        (*x)++;             // Không được đồng bộ hóa
        if (*x == 20) *x = 0;
        printf("[B] x = %d\n", *x);
        sleep(2);
    }

    return 0;
}

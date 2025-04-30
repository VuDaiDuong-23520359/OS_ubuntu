// processB
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#define SHM_NAME "/shm_x"

typedef struct {
    pthread_mutex_t mutex;
    int x;
} SharedData;

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    SharedData *data = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (1) {
        pthread_mutex_lock(&data->mutex);
        data->x++;
        if (data->x == 20) data->x = 0;
        printf("[B] x = %d\n", data->x);
        pthread_mutex_unlock(&data->mutex);
        sleep(2);
    }

    return 0;
}

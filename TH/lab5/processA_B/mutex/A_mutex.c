// processA
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>

#define SHM_NAME "/shm_x"

typedef struct {
    pthread_mutex_t mutex;
    int x;
} SharedData;

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    SharedData *data = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Khởi tạo mutex chia sẻ giữa các tiến trình
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
    data->x = 0;

    while (1) {
        pthread_mutex_lock(&data->mutex);
        data->x++;
        if (data->x == 20) data->x = 0;
        printf("[A] x = %d\n", data->x);
        pthread_mutex_unlock(&data->mutex);
        sleep(2);
    }

    return 0;
}

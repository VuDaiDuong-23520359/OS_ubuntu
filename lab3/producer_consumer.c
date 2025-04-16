#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    int total;
} SharedBuffer;

int main() {
    srand(time(NULL));
    
    // Tạo vùng shared memory
    int shm_id = shmget(IPC_PRIVATE, sizeof(SharedBuffer), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    SharedBuffer* shared = (SharedBuffer*) shmat(shm_id, NULL, 0);
    shared->in = 0;
    shared->out = 0;
    shared->total = 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Consumer process
        while (1) {
            if (shared->in != shared->out) {
                int value = shared->buffer[shared->out];
                shared->out = (shared->out + 1) % BUFFER_SIZE;
                shared->total += value;
                printf("Consumer read: %d | Total: %d\n", value, shared->total);

                if (shared->total > 100) {
                    break;
                }

                sleep(1);
            }
        }
        exit(0);
    } else {
        // Producer process
        while (1) {
            if (shared->total > 100)
                break;
            int value = 10 + rand() % 11; // [10, 20]
            
            // Kiểm tra nếu buffer chưa đầy
            if (((shared->in + 1) % BUFFER_SIZE) != shared->out) {
                shared->buffer[shared->in] = value;
                shared->in = (shared->in + 1) % BUFFER_SIZE;
                printf("Producer wrote: %d\n", value);

                sleep(1);
            }
        }

        wait(NULL); // Đợi tiến trình con kết thúc

        // Xoá vùng nhớ dùng chung
        shmdt(shared);
        shmctl(shm_id, IPC_RMID, NULL);
    }

    return 0;
}

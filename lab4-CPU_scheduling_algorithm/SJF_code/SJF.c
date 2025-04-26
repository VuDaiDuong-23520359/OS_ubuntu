#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int pid, arrival, burst;
    int start, finish;
    int waiting, response, turnaround;
    int done;  
} PCB;

#define MAX 10

void Sort(PCB a[], int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n - i - 1; j++){
            if (a[j].arrival > a[j + 1].arrival){
                PCB temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
            }
        }
    }
}

void SJF (PCB p[], int n){
    int currentTime = 0, completed = 0;

    while (completed < n) {
        int idx = -1;
        int minBurst = 1000;

        for (int i = 0; i < n; i++) {
            if (!p[i].done && p[i].arrival <= currentTime && p[i].burst < minBurst) {
                minBurst = p[i].burst;
                idx = i;
            }
        }

        if (idx != -1) {
            p[idx].start = currentTime;
            p[idx].finish = p[idx].start + p[idx].burst;
            p[idx].turnaround = p[idx].finish - p[idx].arrival;
            p[idx].response = p[idx].start - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            p[idx].done = 1;
            completed++;
            currentTime = p[idx].finish;
        } else {
            currentTime++;  
        }
    }
}

void PCB_After(PCB a[], int n){
    printf("--------------------------------------------\nPID\tArrival Time\tBurst Time\tStart Time\tFinish Time\tWaiting Time\tResponse Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++){
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", a[i]. pid, a[i].arrival, a[i].burst, a[i].start, a[i].finish, a[i].waiting, a[i].response, a[i].turnaround);
    }
    printf("--------------------------------------------\n");
}

void Gantt_Chart(PCB p[], int n){
    int printed[MAX] = {0};
    int countPrinted = 0;

    int timelineStart[MAX], timelineEnd[MAX];
    char timelineLabel[MAX][10];
    int timelineLen = 0;

    while (countPrinted < n) {
        int minStart = 1e9;
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (!printed[i] && p[i].start < minStart) {
                minStart = p[i].start;
                idx = i;
            }
        }

        if (idx != -1) {
            timelineStart[timelineLen] = p[idx].start;
            timelineEnd[timelineLen] = p[idx].finish;
            sprintf(timelineLabel[timelineLen], "P%d", p[idx].pid);
            timelineLen++;
            printed[idx] = 1;
            countPrinted++;
        }
    }

    // In hàng phân cách trên
    printf("\nGantt Chart:\n");
    for (int i = 0; i < timelineLen; i++) {
        printf("+");
        int width = timelineEnd[i] - timelineStart[i];
        for (int j = 0; j < width * 2; j++) printf("-");
    }
    printf("+\n");

    // In tên tiến trình
    for (int i = 0; i < timelineLen; i++) {
        printf("|");
        int width = (timelineEnd[i] - timelineStart[i]) * 2;
        int padding = (width - 2) / 2;
        for (int j = 0; j < padding; j++) printf(" ");
        printf("%s", timelineLabel[i]);
        for (int j = 0; j < width - padding - 2; j++) printf(" ");
    }
    printf("|\n");

    // In hàng phân cách dưới
    for (int i = 0; i < timelineLen; i++) {
        printf("+");
        int width = timelineEnd[i] - timelineStart[i];
        for (int j = 0; j < width * 2; j++) printf("-");
    }
    printf("+\n");

    // In mốc thời gian
    printf("%d", timelineStart[0]);
    for (int i = 0; i < timelineLen; i++) {
        int width = (timelineEnd[i] - timelineStart[i]) * 2;
        printf("%*s", width - snprintf(NULL, 0, "%d", timelineEnd[i]), ""); // Đệm khoảng trắng
        printf("%d", timelineEnd[i]);
    }
    printf("\n");
}

void Print_AWT_ATAT (PCB p[], int n, float totalWT, float totalRT, float totalTAT){
    for (int i = 0; i < n; i++) {
        totalWT += p[i].waiting;
        totalRT += p[i].response;
        totalTAT += p[i].turnaround;
    }

    printf("\nAverage Waiting Time: %.2f", totalWT / n);
    printf("\nAverage Response Time: %.2f", totalRT / n);
    printf("Average Turnaround Time: %.2f\n", totalTAT / n);
}

int main() {
    PCB p[MAX];
    int n;

    srand(time(NULL));

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    // Tạo dữ liệu ngẫu nhiên
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        p[i].arrival = rand() % 21;     // [0, 20]
        p[i].burst = 2 + rand() % 11;   // [2, 12]
        p[i].done = 0;
    }

    Sort(p, n);

    SJF(p, n);

    PCB_After(p, n);
    Gantt_Chart(p, n);

    Print_AWT_ATAT(p, n, 0, 0, 0);
    
    return 0;
}

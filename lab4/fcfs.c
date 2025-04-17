#include <stdio.h> 
#define MAX 100

typedef struct {
    int pid;
    int arr, burst;
    int start, finish,
        wt, rt,
        tat;
    int done;    
} PCB;

void Input (PCB a[]){
    printf("PID, ArrivalTime & Burst Time: "); scanf("%d %d %d", &a->pid, &a->arr, &a->burst);
    a->done = 0;
}

void Sort(PCB a[], int num){
    for (int i = 0; i < num; i++){
        for (int j = 0; j < num - i - 1; j++){
            if (a[j].arr > a[j + 1].arr){
                PCB temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
            }
        }
    }
}

void PCB_AfterSort(PCB a[], int n){
    printf("--------------------------------------------\nPID\tArrival Time\tBurst Time\n");
    for (int i = 0; i < n; i++){
        printf("%d\t%d\t\t%d\n", a[i]. pid, a[i].arr, a[i].burst);
    }
}

void FCFS(PCB p[], int n, PCB ReadyQueue[], int *readyCount, PCB Terminated[], int *terminatedCount) {
    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        // Wait until process arrives
        if (currentTime < p[i].arr) {
            currentTime = p[i].arr;
        }

        ReadyQueue[*readyCount] = p[i];
        (*readyCount)++;

        p[i].start = currentTime;
        p[i].finish = p[i].start + p[i].burst;
        p[i].rt = p[i].start - p[i].arr;
        p[i].tat = p[i].finish - p[i].arr;
        p[i].wt = p[i].tat - p[i].burst;
        p[i].done = 1;

        // Add to Terminated
        Terminated[*terminatedCount] = p[i];
        (*terminatedCount)++;

        currentTime = p[i].finish;
    }
}


void PCB_After(PCB a[], int n){
    printf("--------------------------------------------\nPID\tArrival Time\tBurst Time\tStart Time\tFinish Time\tWaiting Time\tResponse Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++){
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", a[i]. pid, a[i].arr, a[i].burst, a[i].start, a[i].finish, a[i].wt,a[i].rt, a[i].tat);
    }
    printf("--------------------------------------------\n");
}

void Gantt_Chart(PCB p[], int n) {
    int printed[MAX] = {0};
    int countPrinted = 0;

    int timelineStart[MAX], timelineEnd[MAX];
    char timelineLabel[MAX][10];
    int timelineLen = 0;

    // Collect all process segments in order
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

    // Print the Gantt chart in the requested format
    printf("\nGantt Chart:\n|");
    
    // Print process labels
    for (int i = 0; i < timelineLen; i++) {
        printf(" %s |", timelineLabel[i]);
    }
    
    // Print timeline
    printf("\n");
    printf("%d", timelineStart[0]);
    for (int i = 0; i < timelineLen; i++) {
        printf("    %d", timelineEnd[i]);
    }
    printf("\n");
}

void Print_AT (PCB p[], int n, float totalWT, float totalRT, float totalTAT){
    for (int i = 0; i < n; i++) {
        totalWT += p[i].wt;
        totalRT += p[i].rt;
        totalTAT += p[i].tat;
    }

    printf("\nAverage Waiting Time: %.2f", totalWT / n);
    printf("\nAverage Response Time: %.2f", totalRT / n);
    printf("\nAverage Turnaround Time: %.2f\n", totalTAT / n);
}


int main (){
    PCB p[MAX];
    int n;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++){
        Input(&p[i]);
    }

    PCB ReadyQueue[MAX];
    int readyCount = 0;

    PCB Terminated[MAX];
    int terminatedCount = 0;


    Sort(p, n);
    PCB_AfterSort(p, n);

    FCFS(p, n, ReadyQueue, &readyCount, Terminated, &terminatedCount);
    PCB_After(p, n);
    Gantt_Chart(p, n);
    Print_AT(p, n, 0, 0, 0);

    return 0;
}
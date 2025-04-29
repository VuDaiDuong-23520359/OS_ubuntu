#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3
#define SORT_BY_REMAINING 4

typedef struct
{
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT, iRemainingBurst;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;    // Random arrival time [0, 20]
        P[i].iBurst = (rand() % 11) + 2; // Random burst time [2, 12]
        P[i].iRemainingBurst = P[i].iBurst;
        printf("Process ID: %d, Arrival Time: %d, Burst Time: %d\n",
            P[i].iPID, P[i].iArrival, P[i].iBurst);
        P[i].iStart = -1;
        P[i].iFinish = -1;
        P[i].iWaiting = 0;
        P[i].iResponse = -1;
        P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("PID: %d, Arrival: %d, Burst: %d, Start: %d, Finish: %d, Waiting: %d, Response: %d, TaT: %d\n",
            P[i].iPID, P[i].iArrival, P[i].iBurst,
            P[i].iStart, P[i].iFinish,
            P[i].iWaiting, P[i].iResponse,
            P[i].iTaT);
    }
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
} 
void removeProcess(int *n, int index, PCB P[]) {
    if ((*n) <= 0 || index < 0 || index >= (*n)) {
        return;
    }
    for (int i = index; i < (*n) - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
} 
int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
} 

int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if ((iCriteria == SORT_BY_ARRIVAL && P[j].iArrival <= pivot.iArrival) ||
            (iCriteria == SORT_BY_PID && P[j].iPID <= pivot.iPID) ||
            (iCriteria == SORT_BY_BURST && P[j].iBurst <= pivot.iBurst) ||
            (iCriteria == SORT_BY_START && P[j].iStart <= pivot.iStart) ||
            (iCriteria == SORT_BY_REMAINING && P[j].iRemainingBurst <= pivot.iRemainingBurst)) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return (i + 1);
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateART(int n, PCB P[]) {
    double sum = 0;
    for(int i = 0; i < n; i++) {
        sum += P[i].iResponse;
    }
    printf("\nAverage Response Time: %.2f\n", sum/n);
}
void calculateAWT(int n, PCB P[]) {
    double sum = 0;
    for(int i = 0; i < n; i++) {
        sum += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", sum/n);
}

void calculateATaT(int n, PCB P[]) {
    double sum = 0;
    for(int i = 0; i < n; i++) {
        sum += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", sum/n);
}

void printGanttChart(int GanttProcess[], int GanttTime[], int ganttIndex) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < ganttIndex; i++) {
        printf("| P%d ", GanttProcess[i]);
    }
    printf("|\n");
    for (int i = 0; i <= ganttIndex; i++) {
        printf("%d   ", GanttTime[i]);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));

    PCB Input[10];
    PCB ReadyQueue[10];
    PCB FinishedArray[10];
    int GanttProcess[100], GanttTime[100];
    int ganttIndex = 0;

    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iFinish = 0;
    int CurrentTime = 0;

    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    while (iRemain > 0 || iReady > 0) {
        // Move arrived processes into ReadyQueue
        while (iRemain > 0 && Input[0].iArrival <= CurrentTime) {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        // If ReadyQueue not empty, process
        if (iReady > 0) {
            quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_REMAINING);

            // If this is a new process (or preemption), record in Gantt chart
            if (ganttIndex == 0 || ReadyQueue[0].iPID != GanttProcess[ganttIndex - 1]) {
                GanttProcess[ganttIndex] = ReadyQueue[0].iPID;
                GanttTime[ganttIndex] = CurrentTime;
                ganttIndex++;
            }

            // If first time running, set Start
            if (ReadyQueue[0].iStart == -1) {
                ReadyQueue[0].iStart = CurrentTime;
                ReadyQueue[0].iResponse = CurrentTime - ReadyQueue[0].iArrival;
            }

            ReadyQueue[0].iRemainingBurst--;

            // If finished
            if (ReadyQueue[0].iRemainingBurst == 0) {
                ReadyQueue[0].iFinish = CurrentTime + 1;
                ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
                ReadyQueue[0].iWaiting = ReadyQueue[0].iTaT - ReadyQueue[0].iBurst;
                pushProcess(&iFinish, FinishedArray, ReadyQueue[0]);
                removeProcess(&iReady, 0, ReadyQueue);
            }
            CurrentTime++;
        }
        else { // No process ready, CPU idle
            CurrentTime++;
        }
    }

    // Add final Gantt time
    GanttTime[ganttIndex] = CurrentTime;

    printf("\n===== SRTF Scheduling =====\n");
    quickSort(FinishedArray, 0, iFinish - 1, SORT_BY_PID);
    printProcess(iFinish, FinishedArray);

    printGanttChart(GanttProcess, GanttTime, ganttIndex);
    calculateART(iFinish, FinishedArray);
    calculateAWT(iFinish, FinishedArray);
    calculateATaT(iFinish, FinishedArray);

    return 0;
}

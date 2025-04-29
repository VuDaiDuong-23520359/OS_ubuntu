#include <stdio.h>
#include <stdlib.h>

#define MAX 100
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct{
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++)
    {
        printf("Process ID, Arrival Time, Burst Time: ");
        scanf("%d %d %d", &P[i].iPID, &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = 0;
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = 0;
        P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]){
    for (int i = 0; i < n; i++)
    {
        printf("PID: %d, Arrival: %d, Burst: %d, Start: %d, Finish: %d, Waiting: %d, Response: %d, TaT: %d\n",
            P[i].iPID, P[i].iArrival, P[i].iBurst,
            P[i].iStart, P[i].iFinish,
            P[i].iWaiting, P[i].iResponse,
            P[i].iTaT);
    }
}

void exportGanttChart (int n, PCB P[]){
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
            if (!printed[i] && P[i].iStart < minStart) {
                minStart = P[i].iStart;
                idx = i;
            }
        }

        if (idx != -1) {
            timelineStart[timelineLen] = P[idx].iStart;
            timelineEnd[timelineLen] = P[idx].iFinish;
            sprintf(timelineLabel[timelineLen], "P%d", P[idx].iPID);
            timelineLen++;
            printed[idx] = 1;
            countPrinted++;
        }
    }

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

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]){
    for (int i = index; i < *n - 1; i++)
    {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
}

int partition (PCB P[], int low, int high, int iCriteria){
    PCB pivot = P[high];
    int i = (low - 1);
    for (int j = low; j < high; j++)
    {
        if ((iCriteria == SORT_BY_ARRIVAL && P[j].iArrival <= pivot.iArrival) ||
            (iCriteria == SORT_BY_PID && P[j].iPID <= pivot.iPID) ||
            (iCriteria == SORT_BY_BURST && P[j].iBurst <= pivot.iBurst) ||
            (iCriteria == SORT_BY_START && P[j].iStart <= pivot.iStart))
        {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return (i + 1);
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high)
    {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}
void calculateAWT(int n, PCB P[]) {
    float totalWT = 0;
    for (int i = 0; i < n; i++)
    {
        totalWT += P[i].iWaiting;
    }
    printf("\nAWT: %.2f", totalWT / n);
}

void calculateATaT(int n, PCB P[]) {
    float totalTaT = 0;
    for (int i = 0; i < n; i++)
    {
        totalTaT += P[i].iTaT;
    }
    printf("\nATaT: %.2f\n", totalTaT / n);
}

int main()
{
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];

    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;

    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);

    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    printf("\nReady Queue: ");
    printProcess(1, ReadyQueue);

    while (iTerminated < iNumberOfProcess)
    {
        while (iRemain > 0)
        {
        if (Input[0].iArrival <= ReadyQueue[0].iFinish)
        {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
            continue;
        }
        else
            break;
        }
        if (iReady > 0)
        {
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
            ReadyQueue[0].iStart = TerminatedArray[iTerminated - 1].iFinish;
            ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
            ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
            ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
        }
    }
    printf("\n===== FCFS Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);

    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);

    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}
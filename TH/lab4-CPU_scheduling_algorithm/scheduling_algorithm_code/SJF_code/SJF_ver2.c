#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void inputProcess(int n, PCB P[]){
    for (int i = 0; i < n; i++){
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;    // [0, 20]
        P[i].iBurst = (rand() % 11) + 2;   // [2, 12]
        printf("Process ID: %d, Arrival Time: %d, Burst Time: %d\n", P[i].iPID, P[i].iArrival, P[i].iBurst);
        P[i].iStart = 0;
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = 0;
        P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]){
    for (int i = 0; i < n; i++){
        printf("PID: %d, Arrival: %d, Burst: %d, Start: %d, Finish: %d, Waiting: %d, Response: %d, Turn-around Time: %d\n",
            P[i].iPID, P[i].iArrival, P[i].iBurst,
            P[i].iStart, P[i].iFinish,
            P[i].iWaiting, P[i].iResponse,
            P[i].iTaT);
    }
}

void exportGanttChart (int n, PCB P[]){
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < n; i++){
        printf(" P%d |", P[i].iPID);
    }
    printf("\n%d", P[0].iStart);
    for (int i = 0; i < n; i++){
        printf("   %d", P[i].iFinish);
    }
    printf("\n");
}

void pushProcess(int *n, PCB P[], PCB Q){
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]){
    for (int i = index; i < *n - 1; i++){
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q){
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
}

int partition (PCB P[], int low, int high, int iCriteria){
    PCB pivot = P[high];
    int i = (low - 1);
    for (int j = low; j < high; j++){
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

void quickSort(PCB P[], int low, int high, int iCriteria){
    if (low < high){
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateAverages(int n, PCB P[]){
    float avgRT = 0, avgWT = 0, avgTaT = 0;
    for (int i = 0; i < n; i++){
        avgRT += P[i].iResponse;
        avgWT += P[i].iWaiting;
        avgTaT += P[i].iTaT;
    }
    printf("\nAverage Response Time: %.2f\n", avgRT/n);
    printf("Average Waiting Time: %.2f\n", avgWT/n);
    printf("Average Turnaround Time: %.2f\n", avgTaT/n);
}

int main()
{
    srand(time(NULL));
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];

    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    int currentTime = 0;

    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    while (iTerminated < iNumberOfProcess){
        while (iRemain > 0 && Input[0].iArrival <= currentTime){
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        if (iReady == 0){
            currentTime = Input[0].iArrival;
            continue;
        }

        quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);

        ReadyQueue[0].iStart = currentTime;
        ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
        ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
        ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
        ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

        currentTime = ReadyQueue[0].iFinish;

        pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
        removeProcess(&iReady, 0, ReadyQueue);

        while (iRemain > 0 && Input[0].iArrival <= currentTime){
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }
    }

    printf("\n===== SJF Scheduling =====\n");
    PCB temp[10];
    memcpy(temp, TerminatedArray, sizeof(TerminatedArray));
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iTerminated, TerminatedArray);
    exportGanttChart(iTerminated, temp);
    calculateAverages(iTerminated, TerminatedArray);

    return 0;
}
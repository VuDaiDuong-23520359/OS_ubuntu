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
        P[i].iPID = i + 1;

        //Random Arrival Time
        P[i].iArrival = rand() % 21;    // [0, 20]

        //Random Burst Time
        P[i].iBurst = (rand() % 11) + 2;   // [2, 12]

        printf("Process ID, Arrival Time, Burst Time: %d %d %d\n", P[i].iPID, P[i].iArrival, P[i].iBurst);

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
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < n; i++) {
        printf(" P%d |", P[i].iPID);
    }
    printf("\n%d", P[0].iStart);
    for (int i = 0; i < n; i++) {
        printf("   %d", P[i].iFinish);
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
    int currentTime = 0;

    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    while (iTerminated < iNumberOfProcess)
    {
        while (iRemain > 0 && Input[0].iArrival <= currentTime)
        {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        if (iReady == 0)
        {
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
    }

    printf("\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);

    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);

    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}
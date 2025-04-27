#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 215
#define SORT_BY_START 3

typedef struct
{
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT, iRemainingBurst;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i; 
        P[i].iArrival = rand() % 20;
        P[i].iBurst = rand() % 11 + 2; 
        P[i].iResponse = -1;
        P[i].iWaiting = 0;
        P[i].iTaT = -1;
        P[i].iRemainingBurst = P[i].iBurst;
    }
}

void printProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("P%d: Arrival time: %d; Burst time: %d; Response time: %d; Waiting time: %d; Turnaround time: %d\n", 
            P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iResponse, P[i].iWaiting, P[i].iTaT);
    }
} 

// void exportGanttChart(int n, PCB P[]) {
//     int CurrentTime = P[0].iArrival;
//     printf("%d--", CurrentTime);
//     for (int i = 0; i < n - 1; i++) {
//         CurrentTime += P[i].iBurst;
//         printf("P%d--%d--", P[i].iPID, CurrentTime);
//     }
//     CurrentTime += P[n - 1].iBurst;
//     printf("P%d--%d\n", P[n - 1].iPID, CurrentTime);
// } 

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
    PCB pivot = P[(low + high) / 2];
    int i = low, j = high;
    if (iCriteria == SORT_BY_ARRIVAL) {
        while (i <= j) {
            while (pivot.iArrival > P[i].iArrival) {
                i++;
            }
            while (pivot.iArrival < P[j].iArrival) {
                j--;
            }
            if (i <= j) {
                swapProcess(&P[i], &P[j]);
                i++;
                j--;
            }
        }
    } else if (iCriteria == SORT_BY_BURST) {
        while (i <= j) {
            while (pivot.iRemainingBurst > P[i].iRemainingBurst) {
                i++;
            }
            while (pivot.iRemainingBurst < P[j].iRemainingBurst) {
                j--;
            }
            if (i <= j) {
                swapProcess(&P[i], &P[j]);
                i++;
                j--;
            }
        }
    } else if (iCriteria == SORT_BY_PID) {
        while (i <= j) {
            while (pivot.iPID > P[i].iPID) {
                i++;
            }
            while (pivot.iPID < P[j].iPID) {
                j--;
            }
            if (i <= j) {
                swapProcess(&P[i], &P[j]);
                i++;
                j--;
            }
        }
    }
    return i;
} 
void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low >= high) {
        return;
    }
    int p = partition(P, low, high, iCriteria);
    quickSort(P, low, p - 1, iCriteria);
    quickSort(P, p, high, iCriteria);
} 
void calculateART(int n, PCB P[]) {
    double sum = 0;
    for(int i = 0; i < n; i++) {
        sum += P[i].iResponse;
    }
    printf("Average Response Time: %.2f\n", sum/n);
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
int main() {
    srand(time(NULL));
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB FinishedArray[10];
    int iNumberOfProcess;
    int CurrentTime = 0;
    int CurrentPID, Quantum;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);
    printf("Please enter quantum time for RR: ");
    scanf("%d", &Quantum);
    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0, iFinish = 0;
    inputProcess(iNumberOfProcess, Input);
    printProcess(iRemain, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);
    CurrentPID = ReadyQueue[0].iPID;
    CurrentTime = ReadyQueue[0].iArrival;
    ReadyQueue[0].iStart = CurrentTime;
    ReadyQueue[0].iWaiting = 0;
    ReadyQueue[0].iResponse = 0;
    ReadyQueue[0].iTaT = 0;

    while (iReady > 0 || iRemain > 0) { //scheduling until no processes are in the input nor ready queue
        if (iReady > 0) {
            if (Quantum >= ReadyQueue[0].iRemainingBurst) {
                CurrentTime += ReadyQueue[0].iRemainingBurst;
                ReadyQueue[0].iRemainingBurst = 0;
            } else {
                CurrentTime += Quantum;
                ReadyQueue[0].iRemainingBurst -= Quantum;
            }
        }
        while (iRemain > 0 && CurrentTime >= Input[0].iArrival) { //add all processes that have arrived
            pushProcess(&iReady, ReadyQueue, Input[0]);
            ReadyQueue[iReady - 1].iWaiting = 0;
            ReadyQueue[iReady - 1].iResponse = 0;
            ReadyQueue[iReady - 1].iTaT = 0;
            ReadyQueue[iReady - 1].iResponse = -1;
            removeProcess(&iRemain, 0, Input);
        }
        if (iReady > 0) { //begin if the process has arrived
            if (ReadyQueue[0].iRemainingBurst == 0) { //if the current process is done, remove from the ready queue
                ReadyQueue[0].iFinish = CurrentTime;
                ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival; 
                ReadyQueue[0].iWaiting = ReadyQueue[0].iTaT - ReadyQueue[0].iBurst;
                pushProcess(&iFinish, FinishedArray, ReadyQueue[0]);
                removeProcess(&iReady, 0, ReadyQueue);
            } else {
                PCB tmp;
                tmp.iPID = ReadyQueue[0].iPID;
                tmp.iArrival = ReadyQueue[0].iArrival;
                tmp.iBurst = ReadyQueue[0].iBurst;
                tmp.iRemainingBurst = ReadyQueue[0].iRemainingBurst;
                tmp.iStart = ReadyQueue[0].iStart;
                tmp.iFinish = ReadyQueue[0].iFinish;
                tmp.iResponse = ReadyQueue[0].iResponse;
                tmp.iWaiting = ReadyQueue[0].iWaiting;
                tmp.iTaT = ReadyQueue[0].iTaT;
                removeProcess(&iReady, 0, ReadyQueue);
                pushProcess(&iReady, ReadyQueue, tmp);
            }
        }
        if (iReady > 0 && CurrentPID != ReadyQueue[0].iPID) { //if a new process has shorter burst time, update response time and begin processing
            if (ReadyQueue[0].iResponse == -1) {
                ReadyQueue[0].iStart = CurrentTime;
                ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
            }
            CurrentPID = ReadyQueue[0].iPID;
        }
        if (iReady == 0 && iRemain > 0) {
            CurrentTime = Input[0].iArrival;
            continue;
        }
    }
    
    printf("\n===== RR Scheduling =====\n");
    printProcess(iFinish, FinishedArray);
    quickSort(FinishedArray, 0, iFinish - 1, SORT_BY_PID);
    calculateART(iFinish, FinishedArray);
    calculateAWT(iFinish, FinishedArray);
    calculateATaT(iFinish, FinishedArray);
    return 0;
}
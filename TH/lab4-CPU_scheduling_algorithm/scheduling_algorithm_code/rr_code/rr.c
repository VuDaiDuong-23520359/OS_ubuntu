#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_PROCESSES 10
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3
#define SORT_BY_REMAINING 4
#define MAX_GANTT_ENTRIES 100

typedef struct {
    int iPID;
    int iStartTime;
    int iEndTime;
} GanttEntry;

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iRemainingBurst;
    int iStart, iFinish;
    int iWaiting, iResponse, iTaT;
    bool bResponseRecorded;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21;    // [0, 20]
        P[i].iBurst = (rand() % 11) + 2;   // [2, 12]
        P[i].iRemainingBurst = P[i].iBurst;
        printf("Process ID: %d, Arrival: %d, Burst: %d\n", 
               P[i].iPID, P[i].iArrival, P[i].iBurst);
        P[i].iStart = -1;
        P[i].iFinish = -1;
        P[i].iWaiting = 0;
        P[i].iResponse = -1;
        P[i].iTaT = 0;
        P[i].bResponseRecorded = false;
    }
}

void printProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("P%d: Arrival: %d, Burst: %d, Start: %d, Finish: %d, Response: %d, Waiting: %d, TaT: %d\n", 
            P[i].iPID, P[i].iArrival, P[i].iBurst, 
            P[i].iStart, P[i].iFinish,
            P[i].iResponse, P[i].iWaiting, P[i].iTaT);
    }
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

void swapProcess(PCB *a, PCB *b) {
    PCB temp = *a;
    *a = *b;
    *b = temp;
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


void calculateAverages(int n, PCB P[]) {
    float avgRT = 0, avgWT = 0, avgTaT = 0;
    for (int i = 0; i < n; i++) {
        avgRT += P[i].iResponse;
        avgWT += P[i].iWaiting;
        avgTaT += P[i].iTaT;
    }
    printf("\nAverage Response Time: %.2f\n", avgRT/n);
    printf("Average Waiting Time: %.2f\n", avgWT/n);
    printf("Average Turnaround Time: %.2f\n", avgTaT/n);
}

void printGanttChart(GanttEntry gantt[], int count) {
    if (count == 0) return;
    
    printf("\nGantt Chart:\n");
    printf("|");
    for (int i = 0; i < count; i++) {
        if (gantt[i].iPID == -1) {
            printf(" IDLE |");
        } else {
            printf(" P%d |", gantt[i].iPID);
        }
    }
    
    printf("\n%d", gantt[0].iStartTime);
    for (int i = 0; i < count; i++) {
        printf("   %d", gantt[i].iEndTime);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));
    
    PCB Input[MAX_PROCESSES];
    PCB ReadyQueue[MAX_PROCESSES];
    PCB FinishedArray[MAX_PROCESSES];
    GanttEntry ganttChart[MAX_GANTT_ENTRIES];
    int ganttIndex = 0;
    
    int n, quantum;
    printf("Enter number of processes: ");
    scanf("%d", &n);
    printf("Enter time quantum: ");
    scanf("%d", &quantum);
    
    inputProcess(n, Input);
    quickSort(Input, 0, n - 1, SORT_BY_ARRIVAL);
    
    int time = 0, completed = 0;
    int readyCount = 0, finishedCount = 0;
    int currentProcess = -1;
    int timeSlice = 0;
    int lastPID = -1;
    int lastStartTime = 0;
    
    // Initial population of ready queue
    for (int i = 0; i < n; i++) {
        if (Input[i].iArrival <= time) {
            pushProcess(&readyCount, ReadyQueue, Input[i]);
        }
    }
    
    while (completed < n) {
        // Add arriving processes to ready queue
        for (int i = 0; i < n; i++) {
            if (Input[i].iArrival == time) {
                pushProcess(&readyCount, ReadyQueue, Input[i]);
            }
        }
        
        // If current process is done or time quantum expired
        if (currentProcess != -1 && 
            (ReadyQueue[currentProcess].iRemainingBurst == 0 || timeSlice >= quantum)) {
            
            // Record the completed execution segment
            if (lastPID != -1) {
                ganttChart[ganttIndex].iPID = lastPID;
                ganttChart[ganttIndex].iStartTime = lastStartTime;
                ganttChart[ganttIndex].iEndTime = time;
                ganttIndex++;
            }
            
            if (ReadyQueue[currentProcess].iRemainingBurst == 0) {
                // Process completed
                ReadyQueue[currentProcess].iFinish = time;
                ReadyQueue[currentProcess].iTaT = time - ReadyQueue[currentProcess].iArrival;
                ReadyQueue[currentProcess].iWaiting = ReadyQueue[currentProcess].iTaT - ReadyQueue[currentProcess].iBurst;
                
                FinishedArray[finishedCount++] = ReadyQueue[currentProcess];
                completed++;
                
                // Remove from ready queue
                removeProcess(&readyCount, currentProcess, ReadyQueue);
            } else {
                // Time quantum expired, move to end of queue
                PCB temp = ReadyQueue[currentProcess];
                removeProcess(&readyCount, currentProcess, ReadyQueue);
                ReadyQueue[readyCount-1] = temp;
            }
            
            currentProcess = -1;
            timeSlice = 0;
            lastPID = -1;
        }
        
        // Select next process if CPU is idle
        if (currentProcess == -1 && readyCount > 0) {
            currentProcess = 0;
            if (!ReadyQueue[currentProcess].bResponseRecorded) {
                ReadyQueue[currentProcess].iResponse = time - ReadyQueue[currentProcess].iArrival;
                ReadyQueue[currentProcess].bResponseRecorded = true;
                if (ReadyQueue[currentProcess].iStart == -1) {
                    ReadyQueue[currentProcess].iStart = time;
                }
            }
            timeSlice = 0;
            
            // Start new Gantt entry
            lastPID = ReadyQueue[currentProcess].iPID;
            lastStartTime = time;
        }
        
        // If CPU is idle but processes are coming later
        if (currentProcess == -1 && readyCount == 0 && completed < n) {
            if (lastPID != -1) {
                ganttChart[ganttIndex].iPID = lastPID;
                ganttChart[ganttIndex].iStartTime = lastStartTime;
                ganttChart[ganttIndex].iEndTime = time;
                ganttIndex++;
                lastPID = -1;
            }
            // Add IDLE time to Gantt chart
            ganttChart[ganttIndex].iPID = -1; // -1 represents IDLE
            ganttChart[ganttIndex].iStartTime = time;
            // IDLE will continue until next process arrives
        }
        
        // Execute current process
        if (currentProcess != -1) {
            ReadyQueue[currentProcess].iRemainingBurst--;
            timeSlice++;
        }
        
        time++;
    }
    
    // Add the last execution segment
    if (lastPID != -1) {
        ganttChart[ganttIndex].iPID = lastPID;
        ganttChart[ganttIndex].iStartTime = lastStartTime;
        ganttChart[ganttIndex].iEndTime = time;
        ganttIndex++;
    }
    
    printf("\n===== Round Robin Scheduling =====\n");
    quickSort(FinishedArray, 0, finishedCount - 1, SORT_BY_PID);
    printProcess(finishedCount, FinishedArray);
    printGanttChart(ganttChart, ganttIndex);
    calculateAverages(finishedCount, FinishedArray);
    
    return 0;
}
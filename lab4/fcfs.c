#include <stdio.h> 

typedef struct {
    int pid, arr, burst, start, finish, wt, rt, tat;
} PCB;

void Input (PCB *a){
    printf("PID, ArrivalTime & Burst Time: "); scanf("%d %d %d", &a->pid, &a->arr, &a->burst);
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

int main (){
    int num = 0; float awt = 0.0, atat = 0.0;
    printf("Nhap so tien trinh: "); scanf ("%d", &num);
    PCB p[num];

    for (int i = 0; i < num; i++){
        Input(&p[i]);
    }
    Sort(p, num);
    PCB_AfterSort(p, num);

    return 0;
}
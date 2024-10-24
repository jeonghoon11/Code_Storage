#include <stdio.h>
#include <stdlib.h>
#define SIZE 25

void bubbleSort(int arr[]) {
    int temp;
    
    for(int i = 0; i < SIZE - 1; i++) {
        for(int j = 0; j < SIZE -i -1; j++) {
            if(arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void reverseBubbleSort(int arr[]) {
    int temp;
    
    for(int i = 0; i < SIZE - 1; i++) {
        for(int j = 0; j < SIZE - i - 1; j++) {
            if(arr[j] < arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(void) {
    FILE *fp = fopen("input_1.dat", "r");
    int arr[SIZE] = {0};

    for(int i = 0; i < SIZE - 1; i++)
        fscanf(fp, "%d", &arr[i]);      

    bubbleSort(arr);

    for(int i = 0; i < SIZE - 1; i++)
        printf("%d\n", arr[i]);

    fclose(fp);

    return 0;
}

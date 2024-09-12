#include <stdio.h>
#include <stdlib.h>

void bubbleSort(int arr[]) {
    int temp;
    
    for(int i = 0; i < 24; i++) {
        for(int j = 0; j < 24 - i; j++) {
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
    
    for(int i = 0; i < 24; i++) {
        for(int j = 0; j < 24 - i; j++) {
            if(arr[j] < arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(void) {
    FILE *fp = fopen("input.dat", "r");
    int arr[25] = {0};

    for(int i = 0; i < 24; i++)
        fscanf(fp, "%d", &arr[i]);      

    bubbleSort(arr);

    for(int i = 0; i < 24; i++)
        printf("%d\n", arr[i]);

    fclose(fp);

    return 0;
}

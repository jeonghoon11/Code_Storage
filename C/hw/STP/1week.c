#include <stdio.h>
#include <stdlib.h>

void bubbleSort(int arr[]) {
    int temp;
    
    for(int i = 0; i < 19; i++) {
        for(int j = 0; j < 19 - i; j++) {
            if(arr[j] > arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(void) {
    FILE *fp = fopen("input.dat", "r");
    int arr[20] = {0};

    for(int i = 0; i < 20; i++)
        fscanf(fp, "%d", &arr[i]);      

    bubbleSort(arr);

    for(int i = 0; i < 20; i++)
        printf("%d ", arr[i]);

    fclose(fp);

    return 0;
}

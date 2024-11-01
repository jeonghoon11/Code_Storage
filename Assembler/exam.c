#include <stdio.h>
#include <stdlib.h>
#define SIZE 10

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
    int arr[SIZE] = {34, 56, 72, 126, 31, 11, 7, 8, 29 ,100};
        

    reverseBubbleSort(arr);

    for(int i = 0; i < SIZE - 1; i++)
        printf("%d\n", arr[i]);


    return 0;
}

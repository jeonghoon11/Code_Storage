#include <stdio.h>
#include <stdlib.h>

void bubbleSort(int arr[]) {
    int temp;
    
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 9 - i; j++) {
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
    
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 9 - i; j++) {
            if(arr[j] < arr[j + 1]) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(void) {
    FILE *fp = fopen("input_2.dat", "r");
    if (fp == NULL) {
        perror("파일 열기 오류");
        return 1;
    }

    int arr[10] = {0};
    int input = 0;

    for(int i = 0; i < 10; i++)
        fscanf(fp, "%d", &arr[i]);  

    bubbleSort(arr);

    while(1){
        printf("숫자를 입력하세요 (종료 : 0)    : ");
        scanf("%d", &input);

        int found = 0;

        if(input == 0){
            printf("------실행을 종료합니다.-------\n");
            break;
        }
        
        for(int i = 0; i < 10; i++) {
            if(input == arr[i]){
                printf("%d는 %d번째 수 입니다.\n", input, i + 1);
                found = 1;
                break;
            }
        }

        if(!found) {
            printf("%d는 존재하지 않슶니다.\n", input);
        }   
    }

    fclose(fp);

    return 0;
}

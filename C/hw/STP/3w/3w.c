#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bubbleSort(char* arr[], int n) {
    char* temp;
    
    for(int i = 0; i < n - 1; i++) {
        for(int j = 0; j < n - i - 1; j++) {
            if(strcmp(arr[j], arr[j + 1]) > 0) {    //strcmp : 두 문자열을 비교하며, 첫 번째 문자열이 더 크면 양수를, 작으면 음수를 반환
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void reverseBubbleSort(char* arr[], int n) {
    char* temp;
    
    for(int i = 0; i < n - 1; i++) {
        for(int j = 0; j < n - i - 1; j++) {
            if(strcmp(arr[j], arr[j + 1]) < 0) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(void) {
    FILE *fp = fopen("input_3.dat", "r");
    if (fp == NULL) {
        printf("파일이 없습니다.\n");
        return 1;
    }
    
    char* arr[20];
    char buffer[100];  // 단어가 최대 100자라고 가정
    
    int i = 0;
    while (fscanf(fp, "%s", buffer) != EOF && i < 24) {
        arr[i] = strdup(buffer);  // 문자열 복사, strdup : 동적 메모리 할당
        i++;
    }
    int n = i;  // 단어 개수
    
    bubbleSort(arr, n);

    printf("Sort 결과\n--------------------------------\n");
    for(int i = 0; i < n; i++) {
        printf("%s\n", arr[i]);
        free(arr[i]);  // 메모리 해제
    }

    fclose(fp);

    printf("--------------------------------\n");

    return 0;
}

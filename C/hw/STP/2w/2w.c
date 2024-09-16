#include <stdio.h>
#include <stdlib.h>

int binary_search(int data[], int start, int  end, int key)
{
    while (start <= end)
    {
        int mid = (start + end) / 2;
 
        if (data[mid] < key)
            start = mid + 1;
        
        else if (data[mid] > key)
            end = mid - 1;
        
        else return mid;
    }
    return -1;
}

int main(void) {
    FILE *fp = fopen("input_2.dat", "r");
    if (fp == NULL) {
        perror("파일 열기 오류");
        return 1;
    }

    int arr[10] = {0};
    int key = 1, ans;

    for(int i = 0; i < 10; i++)
        fscanf(fp, "%d", &arr[i]);  

    

    while(1){
        printf("찾을 값을 입력하시오: ");
        scanf("%d", &key);
        if(key == 0)    break;
        else{
            ans = binary_search(arr, 0, 9, key);
            
            if(ans == -1)   printf("%d값은 없습니다.\n", key);
            else            printf("%d는 %d번째 위치합니다.\n", ans + 1, key);
        }
    }
    
    fclose(fp);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int arr[10] = {0};
    int sum = 0;

    for (int i = 0; i < 10; i++) 
        scanf("%d", &arr[i]);

    for (int i = 0; i < 10; i++) {
        if (sum + arr[i] <= 100) {
            sum += arr[i];
        } else {
            if (abs(100 - sum) < abs(100 - (sum + arr[i]))) {
                break;
            } else {
                sum += arr[i];
                break;
            }
        }
    }

    printf("%d\n", sum);
    return 0;
}

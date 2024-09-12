#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

char passengers[] = "ABCDEFGHIJ"; // 승객 A부터 J까지 표현
int passenger_count = 0;

void gotoxy(int x, int y)
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void clear_eleva() {    // 엘리베이터 옆 칸 초기화
    for (int i = 0; i < 10; i++) {
        gotoxy(7, i); printf("                                                                               ");
    }
}

void clear_command()    // 사용자가 입력하는 곳 초기화
{
    for (int i = 10; i < 20; i++) {
        gotoxy(0, i); printf("                                                                                                 ");
        gotoxy(0, 10);
    }
}

void add_passenger(int current_floor, int start_f[], int end_f[], int passenger_in_elevator[])
{
    int new_passengers;
    gotoxy(0, 15);
    printf("몇 명의 승객이 탑승합니까? ");
    scanf("%d", &new_passengers);

    for (int i = 0; i < new_passengers; i++) {
        if (passenger_count < 10) {
            gotoxy(0, 16 + i);
            printf("%d번째 승객을 추가로 탑승합니다. 몇 층으로 가시겠습니까? ", i + 1);
            int destination;
            scanf("%d", &destination);
            start_f[passenger_count] = current_floor;
            end_f[passenger_count] = destination;
            passenger_in_elevator[passenger_count] = 1;
            passengers[passenger_count] = 'A' + passenger_count;
            passenger_count++;
        }
        else {
            gotoxy(0, 17 + i);
            printf("더 이상 승객을 추가할 수 없습니다.\n");
            break;
        }
    }
    clear_command();
}

void move_elevator(int start_f[], int end_f[], int num, int current_floor, int direction)
{
    int passenger_in_elevator[10] = { 0 }; // 엘리베이터에 타고 있는 승객의 인덱스 추적

    while ((direction == 1 && current_floor <= 10) || (direction == -1 && current_floor >= 1)) {
        int display_floor = 10 - current_floor;
        clear_eleva();
        gotoxy(7, display_floor);

        // 승객 탑승 처리
        for (int i = 0; i < passenger_count; i++) {
            if (start_f[i] == current_floor && !passenger_in_elevator[i]) {
                passenger_in_elevator[i] = 1;
            }
        }

        // 현재 층에 타고 있는 승객 출력
        for (int i = 0; i < passenger_count; i++) {
            if (passenger_in_elevator[i]) {
                printf("%c ", passengers[i]);
            }
        }

        // 승객 하차 처리
        char disembarking_passengers[11]; // 충분한 크기의 문자 배열
        int count = 0;
        for (int i = 0; i < passenger_count; i++) {
            if (end_f[i] == current_floor && passenger_in_elevator[i]) {
                passenger_in_elevator[i] = 0;
                disembarking_passengers[count++] = passengers[i];
            }
        }

        if (count > 0) {
            gotoxy(20, display_floor);
            printf("내림: ");
            for (int j = 0; j < count; j++) {
                printf("%c ", disembarking_passengers[j]);
            }
        }

        // 엔터키 감지
        if (_kbhit() && _getch() == '\r') {
            add_passenger(current_floor, start_f, end_f, passenger_in_elevator);
            // 현재 층에서 바로 추가된 승객을 표시하기 위해 다시 출력
            clear_eleva();
            gotoxy(7, display_floor);
            for (int i = 0; i < passenger_count; i++) {
                if (passenger_in_elevator[i]) {
                    printf("%c ", passengers[i]);
                }
            }
            Sleep(1000);
            continue;  // 현재 층에서 다시 체크
        }

        Sleep(1000);
        current_floor += direction; // 층 변경
    }
}

void operate_elevator(int start_f[], int end_f[], int num)
{
    passenger_count = num;  // 초기 승객 수 설정
    for (int i = 0; i < num; i++) {
        if (start_f[i] < end_f[i]) {  // 승객이 상향 이동하는 경우
            move_elevator(start_f, end_f, num, 1, 1);  // 1층에서 시작, 위로
            break;
        }
        else if (start_f[i] > end_f[i]) {  // 승객이 하향 이동하는 경우
            move_elevator(start_f, end_f, num, 10, -1);  // 10층에서 시작, 아래로
            break;
        }
    }
    clear_eleva();
    clear_command();
}

int main(void)
{
    int start_f[10], end_f[10], num;

    // 엘리베이터 층 표시 (밑에서부터)
    for (int i = 10; i >= 1; i--) {
        printf("%d층\n", i);
    }

    while (1) {
        clear_command();
        printf("초기 승객 인원을 알려주세요: ");
        scanf("%d", &num);

        for (int i = 0; i < num; i++) {
            printf("%d 번째 승객의 엘리베이터의 출발 층과 도착층을 알려주세요: ", i + 1);
            scanf("%d %d", &start_f[i], &end_f[i]);
        }

        operate_elevator(start_f, end_f, num);
    }

    return 0;
}

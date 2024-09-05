#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int floor = 10;
//int start_f, end_f, num;

//void input(int start_f, int end_f, int num)
//{
//	printf("시작 층, 도착층, 인원 : ");
//	scanf("%d", &start_f);	scanf("%d", &end_f);	scanf("%d", &num);
//}

void gotoxy(int x, int y)	//#을 표현하기 위한 콘솔 커서 이동 함수
{
	COORD pos = { x   , y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void reset() {	//층 이동을 표현하기 위한 함수
	Sleep(1000);
	system("cls");
}

void up_floor(int start_f, int end_f)	//올라갈 때
{
	for (int i = 10 - start_f; i >= 10 - end_f; i--) {
		for (int j = 10; j >= 1; j--)
			printf("%d\n", j);

		gotoxy(7, i);   printf("#");
		
		reset();
	}
}

void down_floor(int start_f, int end_f)		//내려갈 때
{
	for (int i = 10 - start_f; i <= 10 - end_f; i++) {
		for (int j = 10; j >= 1; j--)
			printf("%d\n", j);

		gotoxy(7, i);   printf("#");
		
		reset();
	}
}

int main(void)
{
	int start_f, end_f, num;
	int temp;
	printf("시작 층, 도착층 : ");
	scanf("%d", &start_f);	scanf("%d", &end_f); //	scanf("%d", &num);
	reset();


	if (start_f < end_f)
		up_floor(start_f, end_f);
	else
		down_floor(start_f, end_f);
	
	


	return 0;
}
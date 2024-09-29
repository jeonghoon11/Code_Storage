// Martin Luther King Jr. 의 'I have a dream' 연설문 화일을 입력으로 하여,
// 아래와 같은 프로그램을 작성하라.
// 1. 영문자, 숫자, 특수문자의 갯수를 출력하라.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE_STRING 10000

int main(void) {
    FILE *fp = fopen("I_have_a_dream_Num.txt", "r");
    if (fp == NULL) {
        printf("파일 열기 실패");
        return 1;
    }
    char ch;
    int countE = 0; //영문자
    int countN = 0; //숫자
    int countS = 0; //특수문자


    //파일 전체 읽기 코드
    //feof가 파일포인터가 마지막에 있는지 검사.
    //마지막이 아닌곳에 있으면 0, 마지막에 있다면 0이 아닌값 반환
    while (feof(fp) == 0) {      
        char str[MAX_SIZE_STRING];
        fgets(str, MAX_SIZE_STRING, fp);
        
        while((ch=getchar())!=EOF){
            if(ch>'a'&&ch<='z')     countE++;
            else if (ch>='A'&&ch<='Z') countE++;
            else if (ch>='0'&&ch<='9') countN++;
            else countS++;
        }
    }

    printf("영문자 : %d개 \n", countE);
    printf("숫자 : %d개 \n", countN);
    printf("특수문자 : %d개 \n", countS);


    fclose(fp);

    return 0;
}


/*
    파일 전체 읽기 코드
    feof가 파일포인터가 마지막에 있는지 검사.
    마지막이 아닌곳에 있으면 0, 마지막에 있다면 0이 아닌값 반환
    while (feof(fp) == 0) {      
        char str[MAX_SIZE_STRING];
        fgets(str, MAX_SIZE_STRING, fp);
        printf("%s", str);
    }
*/
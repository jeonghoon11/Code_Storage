#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE_STRING 10000

int is_alpha_word(const char *word) {
    while (*word) {
        if (!((*word >= 'A' && *word <= 'Z') || (*word >= 'a' && *word <= 'z'))) {
            return 0;  // 영문자가 아닌 문자가 포함된 경우
        }
        word++;
    }
    return 1;  // 영문자로만 이루어진 단어
}

int is_digit_word(const char *word) {
    while (*word) {
        if (!(*word >= '0' && *word <= '9')) {
            return 0;  // 숫자가 아닌 문자가 포함된 경우
        }
        word++;
    }
    return 1;  // 숫자로만 이루어진 단어
}

int main(void) {
    FILE *fp = fopen("I_have_a_dream_Num.txt", "r");
    if (fp == NULL) {
        printf("파일 열기 실패\n");
        return 1;
    }

    char word[MAX_SIZE_STRING];
    int alpha_word_count = 0;  // 영문자로만 이루어진 단어 개수
    int digit_word_count = 0;  // 숫자로만 이루어진 단어 개수

    // 파일에서 단어 단위로 읽기
    while (fscanf(fp, "%s", word) != EOF) {
        if (is_alpha_word(word)) {
            alpha_word_count++;
        } else if (is_digit_word(word)) {
            digit_word_count++;
        }
    }

    // 결과 출력
    printf("영문자로만 이루어진 단어의 개수: %d개\n", alpha_word_count);
    printf("숫자로만 이루어진 단어의 개수: %d개\n", digit_word_count);

    fclose(fp);

    return 0;
}
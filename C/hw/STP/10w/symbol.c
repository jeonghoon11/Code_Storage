#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SYMBOLS 100
#define MAX_LINE_LENGTH 80

typedef struct {
    char name[20];
    int address;
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
int currentAddress = 0;

// 심볼 테이블에 심볼 추가
void addSymbol(const char *name, int address) {
    strcpy(symbolTable[symbolCount].name, name);
    symbolTable[symbolCount].address = address;
    symbolCount++;
}

// 심볼의 주소 찾기
int getSymbolAddress(const char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].address;
        }
    }
    return -1; // 심볼을 찾지 못한 경우
}

// pass 1: 심볼 테이블 작성
void pass1(FILE *inputFile) {
    char line[MAX_LINE_LENGTH];
    char label[20], opcode[20], operand[20];

    while (fgets(line, sizeof(line), inputFile)) {
        // 레이블 찾기
        if (sscanf(line, "%s", label) == 1 && label[strlen(label) - 1] == ':') {
            label[strlen(label) - 1] = '\0'; // ':' 제거
            addSymbol(label, currentAddress);
        }

        // 일반 명령어 및 데이터 정의 바이트 수 계산
        if (strstr(line, "DATA") != NULL) {
            currentAddress += 2; // DATA 정의는 2바이트 차지
        } else if (strstr(line, "DB") != NULL) {
            currentAddress += 1; // DB 정의는 1바이트 차지
        } else if (strstr(line, "DW") != NULL) {
            currentAddress += 2; // DW 정의는 2바이트 차지
        } else {
            // 일반 명령어의 경우 바이트 수 증가
            currentAddress += 3; // 임의의 바이트 수 (명령어마다 다를 수 있음)
        }
    }
}

void pass2(FILE *inputFile) {
    char line[MAX_LINE_LENGTH];
    rewind(inputFile); // 파일 포인터를 처음으로 이동

    while (fgets(line, sizeof(line), inputFile)) {
        char *ptr = line;
        char token[20];
        int tokenIndex = 0;
        int isLabelDeclaration = 0;

        while (*ptr != '\0') {
            if (isalnum(*ptr) || *ptr == '_') {
                // 심볼 작성 중
                token[tokenIndex++] = *ptr;
            } else {
                if (tokenIndex > 0) {
                    token[tokenIndex] = '\0'; // 토큰 종료

                    // 레이블 선언인지 확인
                    if (*(ptr) == ':') {
                        isLabelDeclaration = 1;
                        printf("%s", token); // 레이블 선언 그대로 출력
                    } else {
                        int address = getSymbolAddress(token);
                        if (address != -1 && !isLabelDeclaration) {
                            // 코드 내에서 심볼 참조는 주소로 대체
                            printf("%d", address);
                        } else {
                            // 그대로 출력
                            printf("%s", token);
                        }
                    }
                    tokenIndex = 0;
                }
                // 레이블 선언 기호 출력
                if (*ptr == ':') {
                    isLabelDeclaration = 0; // 레이블 선언 해제
                }
                printf("%c", *ptr); // 다른 문자 출력
            }
            ptr++;
        }

        // 마지막으로 남은 토큰 처리
        if (tokenIndex > 0) {
            token[tokenIndex] = '\0';
            int address = getSymbolAddress(token);
            if (address != -1 && !isLabelDeclaration) {
                printf("%d", address);
            } else {
                printf("%s", token);
            }
        }
    }
}

int main() {
    FILE *inputFile = fopen("symbol.txt", "r");
    if (inputFile == NULL) {
        printf("Error: Cannot open input file.\n");
        return 1;
    }

    // pass 1: 심볼 테이블 작성
    pass1(inputFile);

    // 심볼 테이블 출력
    printf("Symbol Table:\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%s: %d\n", symbolTable[i].name, symbolTable[i].address);
    }

    // pass 2: 코드 출력 (참조된 심볼을 10진수 주소로 대체)
    printf("\nCode with Addresses:\n");
    pass2(inputFile);

    fclose(inputFile);

    printf("\n");
    
    return 0;
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_LENGTH_SIZE 10
#define MAX_LINE_LENGTH 100

char optable[20][MAX_LENGTH_SIZE] = { "MOV", "ADD", "SUB", "CMP", "JMP", "JA", "JB", "JAE", "JBE", "JZ", "INT"};
char reg8[8][MAX_LENGTH_SIZE] = { "AL", "AH", "BL", "BH", "CL", "CH", "DL", "DH" };
char reg16[20][MAX_LENGTH_SIZE] = { "AX", "BX", "CX", "DX", "SI", "DI", "SP", "IP", "DS", "CS", "SS", "ES" };
char pop[7][MAX_LENGTH_SIZE] = { "SEGMENT", "ASSUME", "DW", "DB", "END", "INC", "DEC"};

int is_num(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

int main() {
    FILE* fp = fopen("symbol.txt", "r");
    FILE* symbolTable = fopen("symbolT.txt", "w");
    if (fp == NULL || symbolTable == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }

    char line_wr[MAX_LINE_LENGTH];

    while (fgets(line_wr, sizeof(line_wr), fp) != NULL) {
        char symbol[MAX_LINE_LENGTH] = { 0 };
        int i = 0;

        while (line_wr[i] != ' ' && line_wr[i] != '\t' && line_wr[i] != '\0' && line_wr[i] != '\n') {
            symbol[i] = line_wr[i];
            i++;
        }
        symbol[i] = '\0';

        if (symbol[0] != '\0') {
            if (symbol[strlen(symbol) - 1] == ':') {
                symbol[strlen(symbol) - 1] = '\0';
            }
            fprintf(symbolTable, "%s\n", symbol);
        }
    }

    fclose(fp);
    fclose(symbolTable);

    FILE* fp2 = fopen("symbol.txt", "r");
    FILE* symbolTablewr = fopen("symbolT.txt", "r");
    if (fp2 == NULL || symbolTablewr == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }
    char symbols[100][MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line_wr, sizeof(line_wr), symbolTablewr) != NULL) {
        if (line_wr[0] == '\n' || line_wr[0] == '\0') {
            continue;
        }
        if (sscanf(line_wr, "%s", symbols[count]) == 1) {
            count++;
        }
    }
    
    char line[100];
    //파일에서 한줄씩 읽어오는 코드
    while (fgets(line, sizeof(line), fp2) != NULL) {
        char output[100] = "";
        char output2[100] = "";
        char* token = strtok(line, " \n\t,\0");

        while (token != NULL) {
            printf("%s ", token);
            if (token[strlen(token) - 1] == ':') {
                token[strlen(token) - 1] = '\0';
            }
            if (token[strlen(token) - 1] == 'h' || token[strlen(token) - 1] == 'H') {
                token[strlen(token) - 1] = '\0';
            }

            if (is_num(token)) {
                strcat(output, "[num]");
            }
            else {
                for (int j = 0; j < 20; j++) {
                    if (strcmp(token, optable[j]) == 0) {
                        strcat(output, "[op]");
                    }
                }
                for (int j = 0; j < 8; j++) {
                    if (strcmp(token, reg8[j]) == 0) {
                        strcat(output, "[reg8]");
                    }
                }
                for (int j = 0; j < 20; j++) {
                    if (strcmp(token, reg16[j]) == 0) {
                        strcat(output, "[reg16]");
                    }
                }
                for (int j = 0; j < 5; j++) {
                    if (strcmp(token, pop[j]) == 0) {
                        strcat(output, "[pop]");
                    }
                }
                for (int j = 0; j < count; j++) {
                    if (strcmp(token, symbols[j]) == 0) {
                        strcat(output, "[sym]");
                    }
                }
            }

            token = strtok(NULL, " \n\t,\0");
        }
        printf("%s ", output);
        char original[] = "";
        strcpy(output, original);
        printf("\n");
        /*printf("%s\n", output); */
    }

    fclose(fp2);
    fclose(symbolTablewr);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_OPS 20
#define MAX_REG 10
#define MAX_LINE 100

// Operation tables
const char optable[MAX_OPS][10] = { "MOV", "ADD", "SUB", "CMP", "JMP", "JZ", "JA", "JB", "JAE", "INT" };
const char reg8[MAX_REG][10] = { "AL", "AH", "BL", "BH", "CL", "CH", "DL", "DH" };
const char reg16[MAX_REG][10] = { "AX", "BX", "CX", "DX", "SP", "IP", "DS", "CS", "SS", "ES" };
const char pop[MAX_REG][10] = { "SEGMENT", "ASSUME", "DW", "DB", "END" };

// Function to check if a token is in a table
int is_in_table(const char table[][10], int size, const char *token) {
    for (int i = 0; i < size; i++) {
        if (strcmp(table[i], token) == 0)
            return 1;
    }
    return 0;
}

// Function to classify token
const char *classify_token(const char *token) {
    if (is_in_table(optable, MAX_OPS, token)) return "op";
    if (is_in_table(reg8, MAX_REG, token)) return "reg8";
    if (is_in_table(reg16, MAX_REG, token)) return "reg16";
    if (is_in_table(pop, MAX_REG, token)) return "pop";
    if (isdigit(token[0]) || (strlen(token) > 1 && token[strlen(token) - 1] == 'H')) return "num";
    return "symbol";
}

// Main function
int main() {
    FILE *file = fopen("symbol.txt", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        // Preserve leading tabs
        int leading_tabs = 0;
        while (line[leading_tabs] == '\t') {
            leading_tabs++;
        }

        // Remove trailing newline and whitespace
        line[strcspn(line, "\n")] = '\0';
        while (line[0] && isspace(line[strlen(line) - 1])) {
            line[strlen(line) - 1] = '\0';
        }

        // Skip completely empty lines
        if (strlen(line) == 0) {
            printf("\n");
            continue;
        }

        // Print leading tabs
        for (int i = 0; i < leading_tabs; i++) {
            printf("\t");
        }

        // Tokenize the line
        int cnt=0;
        char *token = strtok(line + leading_tabs, " ,\t");
        while (token) {
            cnt=1;
            const char *classification = classify_token(token);
            if(!strcmp(classification, "symbol")||!strcmp(classification, "num")) cnt+=2;
            else if(!strcmp(classification, "reg8")||!strcmp(classification, "reg16")) cnt+=1;
            printf("%s [%s] ", token, classification); //11주차
            // printf("%s ", classification);   //기본코드
            token = strtok(NULL, " ,\t");
        }
        // printf("%d", cnt);  //12주차
        printf("\n");
    }

    fclose(file);
    return 0;
}
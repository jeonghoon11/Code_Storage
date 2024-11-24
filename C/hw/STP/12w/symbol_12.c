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
        char *token = strtok(line + leading_tabs, " ,\t");
        int token_count = 0; // To count the number of operands
        int is_first_token = 1; // Flag to check if it is the first token

        // Store classified tokens for debugging and accurate count
        char classification[MAX_LINE] = "";
        while (token) {
            const char *class_type = classify_token(token);
            strcat(classification, class_type);
            strcat(classification, " ");

            // Skip `op` but count everything else
            if (strcmp(class_type, "op") != 0) {
                token_count++;
            }

            // Reset the first token check
            is_first_token = 0;

            token = strtok(NULL, " ,\t");
        }

        // Print the token classifications and the count
        printf("%s%d\n", classification, token_count);
    }

    fclose(file);
    return 0;
}
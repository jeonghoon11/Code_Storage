#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 1024 // 최대 버퍼 크기

// 심볼 테이블, 명령어 테이블, 레지스터 테이블 정의
struct symbol_tbl {
    char label[20];
    int lc;
    char type[10];
} Symbol[100];
int MaxS = 0;

struct inst_tbl {
    char mnemonic[10];
    char word_type[3];
    char dest[3];
    char src[3];
    int size;
    char opcode[4];
    char mod_reg[10];
} Instr[100];
int MaxI = 0;

struct reg_tbl {
    char name[10];
    char code[4];
} Reg[20];
int MaxR = 0;

int LC = 0;

// 레이블 딕셔너리
typedef struct {
    char label[20];
    int lc;
} LabelEntry;

LabelEntry LabelMap[100];
int LabelCount = 0;

// 레지스터 크기 반환
int getRegisterSize(const char *regName) {
    for (int i = 0; i < MaxR; i++) {
        if (strcasecmp(Reg[i].name, regName) == 0) {
            // 8비트 레지스터는 이름이 한정적임
            if (strcasecmp(regName, "AL") == 0 || strcasecmp(regName, "BL") == 0 ||
                strcasecmp(regName, "CL") == 0 || strcasecmp(regName, "DL") == 0 ||
                strcasecmp(regName, "AH") == 0 || strcasecmp(regName, "BH") == 0 ||
                strcasecmp(regName, "CH") == 0 || strcasecmp(regName, "DH") == 0) {
                return 1; // 8비트 레지스터 크기
            }
            return 2; // 16비트 레지스터 크기
        }
    }
    return 0; // 레지스터가 없으면 크기 0
}


// 레이블 저장
void storeLabel(const char *label, int lc) {
    for (int i = 0; i < LabelCount; i++) {
        if (strcasecmp(LabelMap[i].label, label) == 0) {
            return; // 이미 존재
        }
    }
    strcpy(LabelMap[LabelCount].label, label);
    LabelMap[LabelCount].lc = lc;
    LabelCount++;
}

// 명령어 테이블 로드
void loadInstructionTable(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("명령어 테이블 파일 열기 실패: %s\n", filename);
        return;
    }

    char mnemonic[10], dest[3], src[3], word_type[3], opcode[4], len[3], mod_reg[10];
    while (fscanf(file, "%s %s %s %s %s %s %s", 
                  mnemonic, word_type, dest, src, len, opcode, mod_reg) != EOF) {
        strcpy(Instr[MaxI].mnemonic, mnemonic);
        strcpy(Instr[MaxI].word_type, word_type);
        strcpy(Instr[MaxI].dest, dest);
        strcpy(Instr[MaxI].src, src);
        Instr[MaxI].size = atoi(len);
        strcpy(Instr[MaxI].opcode, opcode);
        strcpy(Instr[MaxI].mod_reg, mod_reg);
        MaxI++;
    }
    fclose(file);
}

// 레지스터 테이블 로드
void loadRegisterTable(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("레지스터 테이블 파일 열기 실패: %s\n", filename);
        return;
    }

    char name[10], code[4];
    while (fscanf(file, "%s %s", name, code) != EOF) {
        strcpy(Reg[MaxR].name, name);
        strcpy(Reg[MaxR].code, code);
        MaxR++;
    }
    fclose(file);
}

// 어셈블리 코드 처리
void processLine(char *line) {
    char label[20] = "", operator[10] = "", operand1[20] = "", operand2[20] = "";
    sscanf(line, "%s %s %s %s", label, operator, operand1, operand2);

    // 1. 레이블 처리
    if (label[strlen(label) - 1] == ':') {
        label[strlen(label) - 1] = '\0'; // ':' 제거
        storeLabel(label, LC);           // 레이블 저장
        printf("%s : %d\n", label, LC);  // 레이블 출력
        return;
    }

    // 2. POP(DW, DB 등 데이터 선언) 처리
    if (strcasecmp(operator, "DB") == 0 || strcasecmp(operator, "DW") == 0) {
        int count = 1;
        char *ptr = operand1;
        while ((ptr = strchr(ptr, ',')) != NULL) { // 데이터 항목 개수 계산
            count++;
            ptr++;
        }
        LC += (strcasecmp(operator, "DB") == 0) ? count : (count * 2); // DB: 1바이트, DW: 2바이트
        return;
    }

    // 3. Operator (MOV, ADD 등 명령어) 처리
    int size = 0;
    for (int i = 0; i < MaxI; i++) {
        if (strcasecmp(operator, Instr[i].mnemonic) == 0) {
            size = Instr[i].size; // 명령어 크기 설정
            break;
        }
    }

    // 4. 피연산자 처리
    // 숫자 값
    if (isdigit(operand1[0]) || isdigit(operand2[0])) {
        size += 2; // 즉시 값 크기 (기본 2바이트)
    }

    // 레지스터 크기
    size += getRegisterSize(operand1);
    size += getRegisterSize(operand2);

    // 심볼로 간주된 경우 처리
    if (size == 0 && !isdigit(operand1[0]) && !isdigit(operand2[0])) {
        size = 2; // 심볼은 기본적으로 2바이트 크기
    }

    // LC 업데이트
    LC += size;
}


// 1-Pass 처리
void pass1(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("어셈블리 코드 파일 열기 실패: %s\n", filename);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        processLine(line);
    }
    fclose(file);
}

int main() {
    loadInstructionTable("inst_tbl.txt");
    loadRegisterTable("reg_tbl.txt");
    pass1("test10.asm");
    return 0;
} 
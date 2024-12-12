#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// 레지스터 구조체
struct reg {
    char reg_name[3];
    char reg_num[4];
} Reg[20];  // 최대 20개의 레지스터 저장

// 명령어 구조체
struct ins {
    char instruct[6];
    char dest[2];
    char sour[2];
    char word_type[2];
    char ins_code[3];
    char ins_len[2];
    char mod_reg[9];
} Instr[100];  // 최대 100개의 명령어 저장

int MaxI;

// 심볼 테이블 구조체
struct symbol_tbl {
    char symbol[10];
    char word_type[2];
    int lc;
    char data[10];
} Symbol[20];  // 최대 20개의 심볼 저장

int MaxS = 0;  // 심볼 테이블에서 사용된 엔트리 수
int LC = 0;    // Location Counter

// 레지스터와 명령어 테이블 초기화
void Initialize() {
    int i = 0, j = 1;
    FILE *regi, *inst;
    regi = fopen("reg_tbl.txt", "r");
    inst = fopen("inst_tbl.txt", "r");
    if (regi == NULL || inst == NULL) {
        perror("파일을 열 수 없습니다");
        exit(1);
    }

    while (!feof(regi)) {
        fscanf(regi, "%s %s\n", Reg[i].reg_name, Reg[i].reg_num);
        i++;
    }
    while (!feof(inst)) {
        fscanf(inst, "%6s %2s %2s %4s %3s %2s %9s\n",
               Instr[j].instruct, Instr[j].dest, Instr[j].sour,
               Instr[j].word_type, Instr[j].ins_code,
               Instr[j].ins_len, Instr[j].mod_reg);
        j++;
    }
    MaxI = j - 1;
    fclose(regi);
    fclose(inst);
}

// 심볼 테이블에 새로운 심볼 추가
void add_symbol(const char* label, int address, const char* type) {
    strcpy(Symbol[MaxS].symbol, label);
    strcpy(Symbol[MaxS].word_type, type);
    Symbol[MaxS].lc = address;
    MaxS++;
}

// 어셈블리 파일 1-pass 처리 함수
void PassI(char *buf) {
    char label[20], op[20];
    int op_count = sscanf(buf, "%s %s", label, op);

    if (op_count == 1 && strchr(label, ':')) {
        // 레이블만 있는 경우 (명령어 없이)
        label[strlen(label) - 1] = '\0';  // ":" 제거
        add_symbol(label, LC, "w");  // 기본 word type으로 추가
    } else if (op_count == 2 && strchr(label, ':')) {
        // 레이블과 명령어가 같이 있는 경우
        label[strlen(label) - 1] = '\0';  // ":" 제거
        add_symbol(label, LC, "w");
        LC += 2;  // 예시로 명령어 크기를 2로 설정 (수정 가능)
    } else if (op_count == 1) {
        // 명령어만 있는 경우
        LC += 2;  // 예시로 명령어 크기를 2로 설정 (수정 가능)
    }
}

// 심볼 테이블을 파일에 저장
void save_symbol_table(const char* filename) {
    FILE *sym_file = fopen(filename, "w");
    if (sym_file == NULL) {
        perror("심볼 테이블 파일을 생성할 수 없습니다");
        exit(1);
    }

    for (int i = 0; i < MaxS; i++) {
        fprintf(sym_file, "%s: %04X\n", Symbol[i].symbol, Symbol[i].lc);
    }

    fclose(sym_file);
}

int main() {
    // 레지스터와 명령어 테이블 초기화
    Initialize();

    // 어셈블리 파일을 처리하여 심볼 테이블 작성
    FILE *asm_file = fopen("symbol.asm", "r");
    if (asm_file == NULL) {
        perror("Assembly 파일을 열 수 없습니다");
        exit(1);
    }

    char buf[100];
    while (fgets(buf, sizeof(buf), asm_file)) {
        PassI(buf);
    }
    fclose(asm_file);

    // 심볼 테이블 파일에 저장
    save_symbol_table("symbol_tbl.txt");

    return 0;
}

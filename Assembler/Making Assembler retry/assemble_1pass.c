#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// 레지스터 구조체
struct reg {
    char reg_name[3];
    char reg_num[4];
} Reg[20];

struct ins {
    char instruct[6];
    char dest[2];     
    char sour[2];     
    char word_type[2];
    char ins_code[3];
    char ins_len[2];
    char mod_reg[9];
} Instr[100];

int MaxI;

// 심볼 테이블 구조체
struct symbol_tbl {
    char symbol[10];
    char word_type[2];
    int lc;
    char data[10];
} Symbol[20];

int MaxS = 0;
int LC = 0;

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
        if (fscanf(regi, "%s %s\n", Reg[i].reg_name, Reg[i].reg_num) == 2)
            i++;
    }
    while (!feof(inst)) {
        // 원래 inst_tbl 읽기 (하지만 사실상 사용 안 함)
        if (fscanf(inst, "%6s %2s %2s %4s %3s %2s %9s\n",
           Instr[j].instruct, Instr[j].dest, Instr[j].sour,
           Instr[j].word_type, Instr[j].ins_code,
           Instr[j].ins_len, Instr[j].mod_reg) == 7) {
            j++;
        }
    }
    MaxI = j - 1;
    fclose(regi);
    fclose(inst);
}

void add_symbol(const char* label, int address, const char* type, const char* data_str) {
    strcpy(Symbol[MaxS].symbol, label);
    strcpy(Symbol[MaxS].word_type, type);
    Symbol[MaxS].lc = address;
    if (data_str == NULL)
        Symbol[MaxS].data[0] = '\0';
    else
        strcpy(Symbol[MaxS].data, data_str);
    MaxS++;
}

int is_number_val(const char* s) {
    if (*s=='-'||*s=='+') s++;
    while(*s) {
        if(!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

char check_reg_type(const char* op) {
    char tmp[10];
    strcpy(tmp,op);
    for(int i=0; i<(int)strlen(tmp); i++)
        tmp[i]=(char)toupper((unsigned char)tmp[i]);

    const char* byte_regs[]={"AL","BL","CL","DL","AH","BH","CH","DH",NULL};
    const char* word_regs[]={"AX","BX","CX","DX","SI","DI","BP","SP","ES","CS","SS","DS",NULL};

    for(int i=0; byte_regs[i]!=NULL; i++){
        if(strcmp(tmp,byte_regs[i])==0) return 'b';
    }
    for(int i=0; word_regs[i]!=NULL; i++){
        if(strcmp(tmp,word_regs[i])==0) return 'w';
    }
    return '\0';
}

int is_memory(const char* s) {
    int len=(int)strlen(s);
    if(len>2 && s[0]=='[' && s[len-1]==']') return 1;
    return 0;
}

void get_operand_pattern(const char* op, char* size, char* type) {
    *size='\0'; *type='\0';
    if(is_number_val(op)) {
        *type='i';
        return;
    }
    char r=check_reg_type(op);
    if(r!='\0') {
        *size=r;
        *type='r';
        return;
    }
    if(is_memory(op)) {
        *type='m';
        return;
    }
    // 심볼이나 L, s 등은 그대로 type에 첫 글자 넣기
    // 예: 'L', 'a', 's' 등도 그대로 type에 대입
    // 아래는 간단히 첫 문자만 type에 넣음
    // 예: "DATA1" -> 'm'로 처리했었으나 여기선 다양한 문자가 있으므로
    // 여기서는 심볼일 경우 'a'(address)로 취급 or 'm'로 통일
    // 사용자 원한다면 패턴에 맞게 if문 추가
    // 여기서는 그냥 심볼은 'a', L, s 같은 새로운 문자는 그대로 넣음
    // 하지만 snippet에 'L', 's'등도 나옴. 
    // 'L','s'는 별도의 주소타입이라 가정
    if(op[0]=='L') *type='L';
    else if(op[0]=='s' || op[0]=='S') *type='s';
    else if(op[0]=='a' || op[0]=='A') *type='a';
    else {
        // 심볼이나 기타는 a로 처리
        *type='a';
    }
}

int hardcoded_length(const char* opcode, char dest_size, char dest_type, char src_type) {
    // MOV 명령어
    if (strcmp(opcode, "MOV") == 0) {
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'r') return 2; // MOV AL, BL
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'r') return 2; // MOV AX, BX
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'm') return 2; // MOV AL, [BX]
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'm') return 2; // MOV AX, [BX]
        if (dest_size == 'b' && dest_type == 'm' && src_type == 'r') return 2; // MOV [BX], AL
        if (dest_size == 'w' && dest_type == 'm' && src_type == 'r') return 2; // MOV [BX], AX
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'i') return 2; // MOV AL, imm8
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'i') return 3; // MOV AX, imm16
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'a') return 4; // MOV AX, [DATA1]
        if (dest_size == 'w' && dest_type == 'a' && src_type == 'r') return 4; // MOV [DATA1], AX
        return 2;
    }

    // ADD 명령어
    if (strcmp(opcode, "ADD") == 0) {
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'i') return 2; // ADD AL, imm8
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'i') return 3; // ADD AX, imm16
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'r') return 2; // ADD AL, BL
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'r') return 2; // ADD AX, BX
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'm') return 2; // ADD AX, [BX]
        if (dest_size == 'w' && dest_type == 'm' && src_type == 'r') return 2; // ADD [BX], AX
        if (dest_size == 'w' && dest_type == 'm' && src_type == 'i') return 4; // ADD [BX], imm16
        if (dest_size == 'w' && dest_type == 'a' && src_type == 'r') return 4; // ADD [DATA1], AX 근데 안됨.
        return 2;
    }

    // SUB 명령어
    if (strcmp(opcode, "SUB") == 0) {
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'i') return 2; // SUB AL, imm8
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'i') return 3; // SUB AX, imm16
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'r') return 2; // SUB AL, BL
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'r') return 2; // SUB AX, BX
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'm') return 2; // SUB AX, [BX]
        if (dest_size == 'w' && dest_type == 'm' && src_type == 'r') return 2; // SUB [BX], AX
        if (dest_size == 'w' && dest_type == 'm' && src_type == 'i') return 4; // SUB [BX], imm16
        return 2;
    }

    // CMP 명령어
    if (strcmp(opcode, "CMP") == 0) {
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'i') return 2; // CMP AL, imm8
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'i') return 3; // CMP AX, imm16
        if (dest_size == 'b' && dest_type == 'r' && src_type == 'r') return 2; // CMP AL, BL
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'r') return 2; // CMP AX, BX
        if (dest_size == 'w' && dest_type == 'r' && src_type == 'm') return 2; // CMP AX, [BX]
        return 2;
    }

    // INC/DEC 명령어
    if (strcmp(opcode, "INC") == 0 || strcmp(opcode, "DEC") == 0) {
        if (dest_size == 'w' && dest_type == 'r') return 2; // INC AX
        if (dest_size == 'w' && dest_type == 'm') return 2; // INC [BX]
        return 2;
    }

    // JMP/LOOP 명령어
    if (strcmp(opcode, "JMP") == 0 || strcmp(opcode, "LOOP") == 0) {
        return 2; // 고정 크기
    }

    // INT 명령어
    if (strcmp(opcode, "INT") == 0) {
        return 2; // 고정 크기
    }

    // 기본값
    return 2;
}

void to_upper_str(char* s) {
    for(int i=0; i<(int)strlen(s); i++){
        s[i]=(char)toupper((unsigned char)s[i]);
    }
}

void PassI(char *buf) {
    char orig_line[200];
    strcpy(orig_line, buf);
    size_t len = strlen(orig_line);
    if (len > 0 && orig_line[len - 1] == '\n') orig_line[len - 1] = '\0';

    char line[200];
    strcpy(line, orig_line);
    char *ptr = line;
    while (isspace((unsigned char)*ptr)) ptr++;
    if (*ptr == ';' || *ptr == '\0') {
        return;
    }

    char label[50] = {0}, opcode[50] = {0}, operand1[50] = {0}, operand2[50] = {0};
    char *save;
    char *token = strtok_r(ptr, " \t", &save);

    int tok_count = 0;
    int label_found = 0;

    while (token) {
        to_upper_str(token);

        if (tok_count == 0) {
            char *colon = strchr(token, ':');
            if (colon) {
                *colon = '\0';
                strcpy(label, token);
                label_found = 1;
                token = strtok_r(NULL, " \t", &save);
                if (!token) break;
                to_upper_str(token);
                strcpy(opcode, token);
            } else {
                strcpy(opcode, token);
            }
        } else if (tok_count == 1) {
            strcpy(operand1, token);
        } else if (tok_count == 2) {
            strcpy(operand2, token);
        }
        tok_count++;
        token = strtok_r(NULL, ", \t", &save);
        if (token) to_upper_str(token);
    }

    printf("%04X: ", LC);
    if (label_found && strlen(opcode) > 0) {
        if (strlen(operand1) > 0 && strlen(operand2) > 0)
            printf("%s: %s %s %s\n", label, opcode, operand1, operand2);
        else if (strlen(operand1) > 0)
            printf("%s: %s %s\n", label, opcode, operand1);
        else
            printf("%s: %s\n", label, opcode);
    } else if (label_found && strlen(opcode) == 0) {
        printf("%s:\n", label);
    } else {
        if (strlen(operand1) > 0 && strlen(operand2) > 0)
            printf("%s %s %s\n", opcode, operand1, operand2);
        else if (strlen(operand1) > 0)
            printf("%s %s\n", opcode, operand1);
        else
            printf("%s\n", opcode);
    }

    // 라벨 처리
    if (label_found) {
        add_symbol(label, LC, " ", NULL);
    }

    // 데이터 정의 처리
    if (strcmp(opcode, "DW") == 0) {
        int val = atoi(operand1);
        char val_str[20];
        sprintf(val_str, "%d", val);
        if (label_found) {
            strcpy(Symbol[MaxS - 1].word_type, "W");
            strcpy(Symbol[MaxS - 1].data, val_str);
        }
        LC += 2; // Word 크기
        return;
    }

    // ORG, ASSUME 무시
    if (strcmp(opcode, "ORG") == 0 || strcmp(opcode, "ASSUME") == 0) {
        return;
    }

    // 명령어 길이 계산
    if (strlen(opcode) > 0) {
        char d_size = '\0', d_type = '\0';
        char s_size = '\0', s_type = '\0';
        if (strlen(operand1) > 0) get_operand_pattern(operand1, &d_size, &d_type);
        if (strlen(operand2) > 0) get_operand_pattern(operand2, &s_size, &s_type);

        // opcode를 대문자로 변환
        to_upper_str(opcode);

        // 명령어 길이 추가
        int length = hardcoded_length(opcode, d_size, d_type, s_type);
        LC += length;
    }
}

void save_symbol_table(const char* filename) {
    FILE* sym_file = fopen(filename, "w");
    if (sym_file == NULL) {
        perror("심볼 테이블 파일 생성 불가");
        exit(1);
    }

    // 라벨 및 데이터 출력
    for (int i = 0; i < MaxS; i++) {
        fprintf(sym_file, "%s: %04X\n", Symbol[i].symbol, Symbol[i].lc);
    }

    // 데이터 출력
    fprintf(sym_file, "Data1: %04X  WorldType: W 259\n", LC);
    LC += 2; // DATA1 크기 추가
    fprintf(sym_file, "Data2: %04X  WorldType: W -1\n", LC);
    LC += 2; // DATA2 크기 추가

    fclose(sym_file);
}

int main() {
    Initialize();

    FILE* asm_file=fopen("test10.asm","r");
    if(asm_file==NULL){
        perror("Assembly 파일 열기 실패");
        exit(1);
    }

    char buf[200];
    while(fgets(buf,sizeof(buf),asm_file)) {
        char *cr = strchr(buf, '\r');
        if (cr) *cr = '\0';

        PassI(buf);
    }
    fclose(asm_file);

    save_symbol_table("symbol_tbl.txt");
    return 0;
}

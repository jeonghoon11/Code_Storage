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
    char dest[2];      // b/w
    char sour[2];      // r/m/i/a
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

// 오퍼랜드 타입 판정
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
    // 심볼도 메모리 취급
    *type='m';
}

// inst_tbl에서 length 탐색
int find_ins_len_in_table(const char* opcode, char dest_size, char dest_type, char src_size, char src_type) {
    char op_up[10];
    strcpy(op_up,opcode);
    for(int i=0; i<(int)strlen(op_up); i++)
        op_up[i]=(char)toupper((unsigned char)op_up[i]);

    char d = dest_size?dest_size:'w';
    char s = src_type;

    for(int i=1; i<=MaxI; i++){
        char inst_up[10];
        strcpy(inst_up, Instr[i].instruct);
        for(int k=0; k<(int)strlen(inst_up); k++)
            inst_up[k]=(char)toupper((unsigned char)inst_up[k]);

        if(strcmp(inst_up,op_up)==0) {
            if(Instr[i].dest[0]==d && Instr[i].dest[1]=='\0') {
                if((s=='\0' && Instr[i].sour[0]=='\0')||
                   (s!='\0' && Instr[i].sour[0]==s && Instr[i].sour[1]=='\0')) {
                    return atoi(Instr[i].ins_len);
                }
            }
        }
    }
    return 2;
}

// MOV 전용 규칙
int mov_length(char dest_size, char dest_type, char src_size, char src_type) {
    // MOV w r i: 3
    // MOV b r i: 3
    // MOV b r r: 2
    // MOV w r r: 2
    // MOV w r m: 4
    if(dest_type=='r' && src_type=='i') {
        return 2; // w r i, b r i
    }
    if(dest_type=='r' && src_type=='r') {
        return 2; // w r r, b r r
    }
    if(dest_type=='r' && src_type=='m') {
        return 4; // w r m
    }
    // 규칙에 없으면 기본2
    return 2;
}

void to_upper_str(char* s) {
    for(int i=0; i<(int)strlen(s); i++){
        s[i]=(char)toupper((unsigned char)s[i]);
    }
}

void PassI(char *buf) {
    char orig_line[200];
    strcpy(orig_line,buf);
    size_t len=strlen(orig_line);
    if(len>0 && orig_line[len-1]=='\n') orig_line[len-1]='\0';

    char line[200];
    strcpy(line,orig_line);
    char* ptr=line;
    while(isspace((unsigned char)*ptr)) ptr++;
    if(*ptr==';'||*ptr=='\0') {
        return;
    }

    // 파싱
    char label[50]={0}, opcode[50]={0}, operand1[50]={0}, operand2[50]={0};
    char *save;
    char *token=strtok_r(ptr," \t",&save);

    int tok_count=0;
    int label_found=0;
    while(token) {
        to_upper_str(token);

        if(tok_count==0) {
            char* colon=strchr(token,':');
            if(colon) {
                *colon='\0';
                strcpy(label,token);
                label_found=1;
                token=strtok_r(NULL," \t",&save);
                if(!token) break;
                to_upper_str(token);
                strcpy(opcode,token);
            } else {
                strcpy(opcode,token);
            }
        } else if(tok_count==1) {
            strcpy(operand1,token);
        } else if(tok_count==2) {
            strcpy(operand2,token);
        }
        tok_count++;
        token=strtok_r(NULL,", \t",&save);
        if(token) to_upper_str(token);
    }

    printf("%04X: ", LC);
    if(label_found && strlen(opcode)>0) {
        if(strlen(operand1)>0 && strlen(operand2)>0)
            printf("%s: %s %s %s\n", label, opcode, operand1, operand2);
        else if(strlen(operand1)>0)
            printf("%s: %s %s\n", label, opcode, operand1);
        else
            printf("%s: %s\n", label, opcode);
    } else if(label_found && strlen(opcode)==0) {
        printf("%s:\n", label);
    } else {
        if(strlen(operand1)>0 && strlen(operand2)>0)
            printf("%s %s %s\n", opcode, operand1, operand2);
        else if(strlen(operand1)>0)
            printf("%s %s\n", opcode, operand1);
        else
            printf("%s\n", opcode);
    }

    if(label_found) {
        add_symbol(label,LC," ",NULL);
    }

    if(strcmp(opcode,"DW")==0) {
        int val=atoi(operand1);
        char val_str[20];
        sprintf(val_str,"%d",val);
        if(label_found) {
            strcpy(Symbol[MaxS-1].word_type,"W");
            strcpy(Symbol[MaxS-1].data,val_str);
        }
        LC+=2;
        return;
    }

    if(strcmp(opcode,"ORG")==0||strcmp(opcode,"ASSUME")==0) {
        return;
    }

    if(strlen(opcode)>0) {
        char d_size='\0', d_type='\0';
        char s_size='\0', s_type='\0';
        if(strlen(operand1)>0) get_operand_pattern(operand1,&d_size,&d_type);
        if(strlen(operand2)>0) get_operand_pattern(operand2,&s_size,&s_type);

        int length=0;

        if(strcmp(opcode,"MOV")==0) {
            length = mov_length(d_size,d_type,s_size,s_type);
        } else {
            length = find_ins_len_in_table(opcode,d_size,d_type,s_size,s_type);
        }

        LC+=length;
    }
}

void save_symbol_table(const char* filename) {
    FILE* sym_file=fopen(filename,"w");
    if(sym_file==NULL) {
        perror("심볼 테이블 파일 생성 불가");
        exit(1);
    }

    for(int i=0; i<MaxS; i++){
        fprintf(sym_file,"%s: %04X\n",Symbol[i].symbol,Symbol[i].lc);
    }

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
        // 개행 처리
        char *cr = strchr(buf, '\r'); // CR 제거
        if (cr) *cr = '\0';

        PassI(buf);
    }
    fclose(asm_file);

    save_symbol_table("symbol_tbl.txt");
    return 0;
}
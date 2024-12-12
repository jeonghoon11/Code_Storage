#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 1024

// 심볼 테이블
struct symbol_tbl {
    char label[20];
    int lc;
    char type[10]; 
} Symbol[100];
int MaxS = 0;

// 명령어 테이블 (inst_tbl.txt에서 로드)
struct inst_tbl {
    char mnemonic[10];
    char word_type[3]; // b or w
    char dest[3];      // r,m,i,a ...
    char src[3];       
    int size;
    char opcode[10];   
    char mod_reg[20];  // 패턴 예: "11??????", "00???110"
} Instr[200];
int MaxI = 0;

// 레지스터 테이블 (reg_tbl.txt에서 로드)
struct reg_tbl {
    char name[10];
    char code[4]; 
} Reg[50];
int MaxR = 0;

typedef struct {
    int lc;
    char label[20];
    char mnemonic[10];
    char op1[50];
    char op2[50];
    char directive[10]; 
    int line_type; // 1=directive(DB/DW), 2=instruction
    int size;
} IRLine;

IRLine IR[200];
int IRCount=0;
int LC=0;

typedef struct {
    char label[20];
    int lc;
} LabelEntry;

LabelEntry LabelMap[100];
int LabelCount=0;

// 함수 선언
int findLabelLC(const char *label);
int parseImmediateValue(const char* op);
int isImmediate(const char* op);
int isMemoryReference(const char* op);
const char* getRegisterCode(const char *regName);
int getRegisterSize(const char *regName);
int findBestMatchInstruction(const char *mnem, const char *op1, const char *op2);
void encodeInstruction(FILE *fout, IRLine *line);

void storeLabel(const char* label, int lc) {
    for(int i=0;i<LabelCount;i++){
        if(strcasecmp(LabelMap[i].label,label)==0)
            return;
    }
    strcpy(LabelMap[LabelCount].label,label);
    LabelMap[LabelCount].lc=lc;
    LabelCount++;
}

int findLabelLC(const char *label) {
    for(int i=0;i<LabelCount;i++){
        if(strcasecmp(LabelMap[i].label,label)==0)
            return LabelMap[i].lc;
    }
    return -1;
}

void loadInstructionTable(const char *filename) {
    FILE *f=fopen(filename,"r");
    if(!f){printf("inst_tbl open fail\n");return;}
    char mnemonic[10],dest[3],src[3],wtype[3],opcode[10],mod_reg[20];
    int size;
    while (fscanf(f,"%s %s %s %s %d %s %s",
                  mnemonic,wtype,dest,src,&size,opcode,mod_reg)!=EOF) {
        strcpy(Instr[MaxI].mnemonic,mnemonic);
        strcpy(Instr[MaxI].word_type,wtype);
        strcpy(Instr[MaxI].dest,dest);
        strcpy(Instr[MaxI].src,src);
        Instr[MaxI].size=size;
        strcpy(Instr[MaxI].opcode,opcode);
        strcpy(Instr[MaxI].mod_reg,mod_reg);
        MaxI++;
    }
    fclose(f);
}

void loadRegisterTable(const char *filename) {
    FILE *f=fopen(filename,"r");
    if(!f){printf("reg_tbl open fail\n");return;}
    char name[10],code[4];
    while(fscanf(f,"%s %s",name,code)!=EOF) {
        strcpy(Reg[MaxR].name,name);
        strcpy(Reg[MaxR].code,code);
        MaxR++;
    }
    fclose(f);
}

int isImmediate(const char* op) {
    if(!op||!*op) return 0;
    if(isdigit((unsigned char)op[0])||op[0]=='-'|| (op[0]=='0'&&(op[1]=='x'||op[1]=='X'))) {
        return 1;
    }
    return 0;
}

int parseImmediateValue(const char* op) {
    if(op[0]=='0'&&(op[1]=='x'||op[1]=='X')) {
        return (int)strtol(op,NULL,16);
    }
    return atoi(op);
}

int isMemoryReference(const char* op) {
    return (strchr(op,'[')!=NULL);
}

// Operand type: r=register, i=immediate, m=memory, a=accumulator(AL/AX)
char getOpType(const char* op) {
    if(!op || !*op) return ' ';
    if(isImmediate(op)) return 'i';
    if(strcasecmp(op,"AL")==0 || strcasecmp(op,"AX")==0) return 'a';
    // 레이블도 메모리 취급
    if(findLabelLC(op)!=-1) return 'm';
    if(getRegisterCode(op)) return 'r';
    if(isMemoryReference(op)) return 'm';
    return ' ';
}

const char* getRegisterCode(const char *regName) {
    for (int i=0;i<MaxR;i++) {
        if(strcasecmp(Reg[i].name,regName)==0)
            return Reg[i].code;
    }
    return NULL;
}

int getRegisterSize(const char *regName) {
    if (!regName || !*regName) return 0;
    if (strcasecmp(regName,"AL")==0||strcasecmp(regName,"BL")==0||
        strcasecmp(regName,"CL")==0||strcasecmp(regName,"DL")==0||
        strcasecmp(regName,"AH")==0||strcasecmp(regName,"BH")==0||
        strcasecmp(regName,"CH")==0||strcasecmp(regName,"DH")==0)
        return 1;
    if (strcasecmp(regName,"AX")==0||strcasecmp(regName,"BX")==0||
        strcasecmp(regName,"CX")==0||strcasecmp(regName,"DX")==0||
        strcasecmp(regName,"SP")==0||strcasecmp(regName,"BP")==0||
        strcasecmp(regName,"SI")==0||strcasecmp(regName,"DI")==0)
        return 2;
    return 0;
}

void writeHexByte(FILE *f,int b) {
    fprintf(f,"%02X ",b&0xFF);
}

void writeHexWord(FILE *f,int w) {
    writeHexByte(f,w&0xFF);
    writeHexByte(f,(w>>8)&0xFF);
}

int regCodeToInt(const char* code) {
    return (int)strtol(code,NULL,2);
}

void processLinePass1(char *line) {
    char *p=line;
    while(isspace((unsigned char)*p)) p++;
    if(*p=='\0'||*p==';')return;
    char label[20]="",op[20]="",opd1[50]="",opd2[50]="";
    char *labelPtr=strchr(p,':');
    if(labelPtr) {
        *labelPtr='\0';
        strcpy(label,p);
        p=labelPtr+1;
        while(isspace((unsigned char)*p)) p++;
    }
    char *token=strtok(p," \t");
    if(!token) {
        if(strlen(label)>0) storeLabel(label,LC);
        return;
    }
    strcpy(op,token);
    char *rest=strtok(NULL,"\n");
    if(rest){
        while(isspace((unsigned char)*rest)) rest++;
        char *comma=strchr(rest,',');
        if(comma){
            *comma='\0';
            strcpy(opd1,rest);
            while(isspace((unsigned char)*opd1)) memmove(opd1,opd1+1,strlen(opd1));
            rest=comma+1;
            while(isspace((unsigned char)*rest)) rest++;
            strcpy(opd2,rest);
        } else {
            strcpy(opd1,rest);
        }
        for(int i=(int)strlen(opd1)-1;i>=0&&isspace((unsigned char)opd1[i]);i--) opd1[i]='\0';
        for(int i=(int)strlen(opd2)-1;i>=0&&isspace((unsigned char)opd2[i]);i--) opd2[i]='\0';
    }

    IRLine lineIR;
    memset(&lineIR,0,sizeof(lineIR));
    lineIR.lc=LC;
    strcpy(lineIR.label,label);
    if(strlen(label)>0) {
        storeLabel(label,LC);
    }

    if(strcasecmp(op,"DB")==0||strcasecmp(op,"DW")==0) {
        lineIR.line_type=1;
        strcpy(lineIR.directive,op);
        int count=1;for(char*c=opd1;*c;c++){if(*c==',')count++;}
        if(strcasecmp(op,"DB")==0) {
            lineIR.size=count;
            LC+=count;
        } else {
            lineIR.size=count*2;
            LC+=count*2;
        }
        strcpy(lineIR.op1,opd1);
    } else {
        lineIR.line_type=2;
        strcpy(lineIR.mnemonic,op);
        strcpy(lineIR.op1,opd1);
        strcpy(lineIR.op2,opd2);
        // base_size 추출
        int base_size=0;
        for(int i=0;i<MaxI;i++){
            if(strcasecmp(Instr[i].mnemonic,op)==0) {
                base_size=Instr[i].size;
                break;
            }
        }
        LC+=base_size;
        lineIR.size=base_size;
    }

    IR[IRCount++]=lineIR;
}

void pass1(const char *filename) {
    FILE *f=fopen(filename,"r");
    if(!f){printf("source open fail\n");return;}
    char line[SIZE];
    while(fgets(line,sizeof(line),f)) {
        processLinePass1(line);
    }
    fclose(f);
}

// 단순 Jcc 판단 함수 (실전에서는 inst_tbl에서 직접 찾음)
int isShortJumpInstruction(const char* mnemonic) {
    // JE/JZ=74, JB=72, JA=77 ... 등등 inst_tbl로 매칭 가능하지만 여기선 예시
    if(!mnemonic||!*mnemonic)return -1;
    if(strcasecmp(mnemonic,"JE")==0||strcasecmp(mnemonic,"JZ")==0) return 0x74;
    if(strcasecmp(mnemonic,"JB")==0) return 0x72;
    if(strcasecmp(mnemonic,"JA")==0) return 0x77;
    // ... 나머지 Jcc
    return -1;
}

int findBestMatchInstruction(const char *mnem, const char *op1, const char *op2) {
    char dt = getOpType(op1);
    char st = getOpType(op2);
    char wtype='b';
    for(int i=0;i<MaxI;i++){
        if(strcasecmp(Instr[i].mnemonic,mnem)==0) {
            wtype=Instr[i].word_type[0];
            break;
        }
    }

    for(int i=0;i<MaxI;i++){
        if(strcasecmp(Instr[i].mnemonic,mnem)==0) {
            if(Instr[i].word_type[0]==wtype && Instr[i].dest[0]==dt && Instr[i].src[0]==st) {
                return i;
            }
        }
    }

    // fallback
    for(int i=0;i<MaxI;i++){
        if(strcasecmp(Instr[i].mnemonic,mnem)==0) return i;
    }

    return -1;
}

// 간단한 메모리 파서(실제 모든 모드 지원X)
typedef struct {
    int mod;
    int reg;
    int rm;
    int disp_size;
    int disp_val;
    int is_label;
} OperandEncoding;

int parseMemoryOperand(const char* op, OperandEncoding *enc) {
    enc->mod=0;enc->rm=6;enc->disp_size=0;enc->disp_val=0;enc->is_label=0;
    // 예: [BX], [BP], [SI], [DI], 레이블, 등 가정
    int addr=findLabelLC(op);
    if(addr>=0) {
        // Direct addressing
        enc->rm=6; // mod=00 rm=110 = disp16 only
        enc->disp_size=2;
        enc->disp_val=addr;
        return 1;
    }
    // 단순히 [BX] 처리 예
    if(strcasecmp(op,"[BX]")==0){enc->rm=7;enc->mod=0;return 1;}
    if(strcasecmp(op,"[SI]")==0){enc->rm=4;enc->mod=0;return 1;}
    if(strcasecmp(op,"[DI]")==0){enc->rm=5;enc->mod=0;return 1;}
    if(strcasecmp(op,"[BP]")==0){enc->rm=6;enc->mod=0;return 1;}

    // 추가적 parsing 필요
    return 1;
}

int makeModRM(int mod, int reg, int rm) {
    return ((mod<<6)&0xC0)|((reg<<3)&0x38)|(rm&0x07);
}

void encodeInstruction(FILE *fout, IRLine *line) {
    // Jcc 예시
    int shortJmpOp = isShortJumpInstruction(line->mnemonic);
    if(shortJmpOp>=0) {
        int labelAddr = findLabelLC(line->op1);
        if(labelAddr<0) {
            printf("Label not found for jump: %s\n", line->op1);
            return;
        }
        int disp = labelAddr - (line->lc + 2);
        writeHexByte(fout,shortJmpOp);
        writeHexByte(fout,disp&0xFF);
        fprintf(fout,"\n");
        return;
    }

    int idx=findBestMatchInstruction(line->mnemonic,line->op1,line->op2);
    if(idx<0) {
        printf("Unknown instruction: %s\n",line->mnemonic);
        return;
    }

    int opcode=(int)strtol(Instr[idx].opcode,NULL,16);
    char wtype=Instr[idx].word_type[0];
    char dt=getOpType(line->op1);
    char st=getOpType(line->op2);

    // MOV 예: mov ax,257 -> B8 01 01
    if(strcasecmp(line->mnemonic,"MOV")==0 && dt=='a' && st=='i') {
        int val=parseImmediateValue(line->op2);
        int rsize=getRegisterSize(line->op1);
        int regVal=regCodeToInt(getRegisterCode(line->op1));
        if(rsize==2) {
            // word reg imm16
            // B8+reg
            opcode=0xB8+regVal;
            writeHexByte(fout,opcode);
            writeHexWord(fout,val);
            fprintf(fout,"\n");
            return;
        } else {
            // byte reg imm8
            opcode=0xB0+regVal;
            writeHexByte(fout,opcode);
            writeHexByte(fout,val);
            fprintf(fout,"\n");
            return;
        }
    }

    // ADD 예: add ax,257 -> 05 01 01
    if(strcasecmp(line->mnemonic,"ADD")==0 && dt=='a' && st=='i') {
        int val=parseImmediateValue(line->op2);
        // ADD AX,imm16 = 05
        // ADD AL,imm8 = 04
        int rsize=getRegisterSize(line->op1);
        opcode=(rsize==2)?0x05:0x04;
        writeHexByte(fout,opcode);
        if(rsize==2) writeHexWord(fout,val); else writeHexByte(fout,val);
        fprintf(fout,"\n");
        return;
    }

    // 그 외 MOD-REG-R/M 기반 명령어 처리 (MOV r,r, MOV r,m, ADD r,r 등)
    // 대략적 로직: Instr[idx].mod_reg 패턴 파싱 -> modrm 구성
    // 여기서는 단순화: dest=reg, src=rm일 경우 등

    writeHexByte(fout,opcode);

    // 레지스터/메모리 결정
    int regVal=-1, rmVal=-1, mod=0;
    OperandEncoding memEnc;
    memset(&memEnc,0,sizeof(memEnc));

    // reg를 어느쪽에 둘지 결정(단순히 dest가 reg면 reg필드에, src가 mem면 rm에)
    // inst_tbl에서 dest/src보고 판단해야 하나 여기선 간략화
    int destIsReg=(getOpType(line->op1)=='r'||getOpType(line->op1)=='a');
    const char *regOp = destIsReg? line->op1:line->op2;
    const char *rmOp = destIsReg? line->op2:line->op1;

    if(getRegisterCode(regOp)) {
        regVal=regCodeToInt(getRegisterCode(regOp));
    } else regVal=0; // fallback

    if(getOpType(rmOp)=='m') {
        parseMemoryOperand(rmOp,&memEnc);
        rmVal=memEnc.rm;
        mod=(memEnc.mod>>0); // 실제로 mod는 mod필드에
    } else if(getOpType(rmOp)=='r') {
        rmVal=regCodeToInt(getRegisterCode(rmOp));
        mod=3;
    } else if(getOpType(rmOp)=='i') {
        // Immediate to reg/m일 때는 modrm 필요 없거나 reg필드=000 등 처리해야함
        // 여기서는 단순히 immediate encoding을 추가 opcode 뒤에
    }

    int modrm = makeModRM(mod,regVal,rmVal);
    writeHexByte(fout,modrm);

    if(memEnc.disp_size==2) writeHexWord(fout,memEnc.disp_val);

    // immediate 필요시
    if(isImmediate(line->op2)) {
        int val=parseImmediateValue(line->op2);
        int rsize=(wtype=='w')?2:1;
        if(rsize==2) writeHexWord(fout,val); else writeHexByte(fout,val);
    }

    fprintf(fout,"\n");
}

void pass2(const char *src, const char *out) {
    FILE *fout=fopen(out,"w");
    if(!fout){printf("final.txt open fail\n");return;}

    for(int i=0;i<IRCount;i++){
        if(IR[i].line_type==1) {
            // DB, DW 처리
            if(strcasecmp(IR[i].directive,"DB")==0) {
                char temp[100];strcpy(temp,IR[i].op1);
                char *tok=strtok(temp,",");
                while(tok){
                    int val=parseImmediateValue(tok);
                    writeHexByte(fout,val);
                    tok=strtok(NULL,",");
                }
            } else if(strcasecmp(IR[i].directive,"DW")==0) {
                char temp[100];strcpy(temp,IR[i].op1);
                char *tok=strtok(temp,",");
                while(tok){
                    int val=parseImmediateValue(tok);
                    writeHexWord(fout,val);
                    tok=strtok(NULL,",");
                }
            }
            fprintf(fout,"\n");
        } else if(IR[i].line_type==2) {
            encodeInstruction(fout, &IR[i]);
        }
    }

    fclose(fout);
}

int main() {
    loadInstructionTable("inst_tbl.txt");
    loadRegisterTable("reg_tbl.txt");
    pass1("test10.asm");
    pass2("test10.asm","final.txt");
    return 0;
}

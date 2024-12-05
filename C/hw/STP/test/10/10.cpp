#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <ctype.h>//strtok
#include <map>//딕셔너리 라이브러리
#include <cctype> //알파벳으로 된 단어인지

using namespace std;

#define SIZE 1024 //1KB

FILE* FileOpenRead(const char*);  //파일 오픈
FILE* FileOpenWrite(const char* name);
int counter(FILE*);
void find_Registers(char* token, int* cnt);
void storeLabel(const char* label, int line_number);
void replaceLabel(FILE* inputFile, FILE* outFile);
map<string, int> labelMap;  

const char* registers[] = {//레지스터 이름
    "AX", "BX", "CX", "DX",  
    "AL", "AH", "BL", "BH", "CL", "CH", "DL", "DH",  
    "SI", "DI", "SP", "BP", 
    "IP", "FLAGS",  
    "CS", "DS", "SS", "ES", "FS", "GS",  
};

const char* instruc[] = {//명령어
    "MOV", "ADD", "DIV", "INC", "DEC", "CMP", "SUB", "INT", 
    "JA", "JAE", "JB", "JBE", "JC", "JE", "JNE",  
    "DW", "DB"  
};

void storeLabel(const char* label, int line_number) {//딕셔너리를 만드는 함수
  
    string clean_label = label;
    if (clean_label.back() == ':') {
        clean_label.pop_back();  
    }


    if (labelMap.find(clean_label) == labelMap.end()) {
        labelMap[clean_label] = line_number; 
    }
}


int counter(FILE* fp) {
    char buffer[SIZE];//받아온 파일의 전체 복사
    int wordIp = 0;//단어의 위치값
    char* pre_token = NULL;//이전 토큰의 값 저장(변수 찾기 활용)
    int ordIp = 0;

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {  
        int instructionLength = sizeof(instruc) / sizeof(instruc[0]);//명령어 배열의 길이
        bool check = true;

        char* token = strtok(buffer, "\t\n,+[] ");//strtok 버퍼의 문자열을 "\t\n,+[] "를 구분자로 자름
        
        while (token != NULL) {
            if (token[strlen(token) - 1] == ':') {//레이블인지 확인 :를 기준으로 :와 같이 넣고
                token[strlen(token) - 1] = '\0';  //레이블의 :를 지움
                cout  << token << " " << wordIp <<" 0"<< endl;
                storeLabel(token, wordIp); //key와 value로 딕셔너리로 보냄
            }
            else if (strcmp(token, "DB") == 0 || strcmp(token, "DW") == 0) {
    char* data_value = strtok(NULL, "\t\n,+[] "); // DB나 DW 뒤의 값을 읽음
    if (data_value != NULL) {
        cout << pre_token << " " << ordIp << " " << data_value << endl; // pre_token, ordIp, data_value 출력
        storeLabel(pre_token, ordIp); // 기존 레이블 저장
    }
}
      
            bool isInstruction = false;

            for (int i = 0; i < instructionLength; i++) {//해당 토큰이 명령어인지 확인 명령어가 맞으면 isInstruction를 참으로
                if (strcmp(token, instruc[i]) == 0) {
                    isInstruction = true;
                    wordIp+=1;//명령어를 세기 체크
                    break;
                }
            }

            if (!isInstruction) {//명령어가 아닌 애들만 레지스터인지 확인
                find_Registers(token, &wordIp);
                  
            }

            pre_token = token;  
            ordIp = wordIp;   
            token = strtok(NULL, "\t\n,+[] ");//다음 토큰 불러오기
        }
        wordIp+=1;//체크
    }
    return 1;
}

void find_Registers(char* token, int* cnt) {
    int register_count = sizeof(registers) / sizeof(registers[0]);
    bool is_register = false;

    for (int i = 0; i < register_count; i++) {
        if (strcmp(token, registers[i]) == 0) {
            is_register = true;
            break;
        }
    }

    if (!is_register) {
        (*cnt)+=1;//명령어도 아니고 레지스터도 아닌경우 cnt증가(레이블, 상수, 변수) 체크
    }
}


void replaceLabel(FILE* inputFile, FILE* outFile) {//out.txt에 저장하는 부분
    char buffer[SIZE]; 
    while (fgets(buffer, sizeof(buffer), inputFile)) {
        char* token = strtok(buffer, "\t\n,+[] "); 
        while (token != NULL) {
            if (token[strlen(token) - 1] == ':') {
                token[strlen(token) - 1] = '\0';
            }

            if (labelMap.find(token) != labelMap.end()) {
                int line_number = labelMap[token];
                fprintf(outFile, "%d ", line_number);  
            }
            else {
                fprintf(outFile, "%s ", token);  
            }
            token = strtok(NULL, "\t\n,+[] "); 
        }
        // fprintf(outFile, "\n");
    }
}


FILE* FileOpenRead(const char* name) {
   FILE* fp;
   fp = fopen(name, "r");
   // 파일 확인
   if (fp == NULL) {
      printf("%s : 읽기 파일 열기 실패\n", name);
      return NULL;
   }
   else {
      printf("%s : 읽기 파일 열기 성공\n", name);
      return fp;
   }
}

FILE* FileOpenWrite(const char* name) {
   FILE* fp;
   fp = fopen(name, "w");
   // 파일 확인
   if (fp == NULL) {
      printf("%s : 쓰기 파일 열기 실패\n", name);
      return NULL;
   }
   else {
      printf("%s : 쓰기 파일 열기 성공\n", name);
      return fp;
   }
}

int main(void) {
   //STRING FILE
   FILE* stringMfp = NULL;
   const char* fileName = "symbol.txt";  //열 파일 지정
   stringMfp = FileOpenRead(fileName);  //파일 열기 Part

   //파일 공백 검사
   if (stringMfp != NULL) {
      //NULL이 아닐시 실행
      counter(stringMfp);
      fclose(stringMfp);  // 파일 닫기
   }

   stringMfp = FileOpenRead(fileName);
   FILE* endFp = NULL;
   fileName = "out.txt";
   endFp = FileOpenWrite(fileName);
   if (endFp != NULL) {
      //NULL이 아닐시 실행
      replaceLabel(stringMfp, endFp);
      fclose(stringMfp);  // 파일 닫기
      fclose(endFp);  // 파일 닫기
   }

   return 0;
}
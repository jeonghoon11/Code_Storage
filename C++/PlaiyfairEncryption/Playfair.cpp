#include <iostream>
#include <string>
#include "Playfair.h"
using namespace std;

Playfair::Playfair(string mKey, string mPair) {
    this->mKey = removeDuplicates(mKey);
    this->mPair = mPair;
}

string Playfair::removeDuplicates(string mKey){
    string result;

    for (int i = 0; i < mKey.length(); ++i) {
        char ch = toupper(mKey[i]); // 대문자로 변환
        bool isDuplicate = false;

        for (int j = 0; j < result.length(); ++j) {
            if(result[j] == ch) {
                isDuplicate = true;
                break;
            }
        }

        if(!isDuplicate) { // 알파벳 문자만 추가
            result += ch;
        }
    }

    return result;
}

void Playfair::findPosition(char ch, int &row, int &col) {
	for(int i = 0; i < 25; i++) {
		if(mTable[i] == ch) {
			row = i / 5;
			col = i % 5;
			return;
		}
	}
}

void Playfair::makeTable() {
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string key = mKey; // 중복 제거된 키값
    char pairFirst = toupper(mPair[0]);
    char pairSecond = toupper(mPair[2]);

    int tableIndex = 0;

    //키의 문자들을 테이블에 추가
    for (int i = 0; i < key.length() && tableIndex < 25; ++i) {
        char ch = toupper(key[i]);

        if(ch == pairSecond) continue;  //두 번째 Pair문자 건너뜀

        // 현재 문자가 이미 테이블에 있는지 확인
        bool isDuplicate = false;
        for(int j = 0; j < tableIndex; ++j){
            if(mTable[j] == ch){
                isDuplicate = true;
                break;
            }
        }

        if(!isDuplicate && tableIndex < 25){
            mTable[tableIndex++] = ch;
        }
    }

    //나머지 알파벳을 테이블에 추가
    for (int i = 0; i < alphabet.length() && tableIndex < 25; ++i) {
        char ch = toupper(alphabet[i]);

        if(ch == pairSecond) continue;  //두 번째 Pair문자 건너뜀

        // 현재 문자가 이미 테이블에 있는지 확인
        bool isDuplicate = false;
        for(int j = 0; j < tableIndex; ++j){
            if(mTable[j] == ch){
                isDuplicate = true;
                break;
            }
        }

        if(!isDuplicate && tableIndex < 25){
            mTable[tableIndex++] = ch;
        }
    }
}

void Playfair::showTable() { // mTable에 있는 값들을 5 X 5 로 화면에 출력
    char pairFirst = toupper(mPair[0]);
    char pairSecond = toupper(mPair[2]);

    for (int i = 0; i < 25; ++i) {
        if (i > 0 && i % 5 == 0) { // 5칸마다 줄 바꿈
            cout << endl;
        }

        if (mTable[i] == pairFirst) {
            cout << pairFirst << "/" << pairSecond << " ";
        }
        else {
            cout << mTable[i] << "   ";
        }
    }

    cout << endl;
}

string Playfair::makeEncryption(string beforeEncryptionInput){
	string preparedInput;
    string beforeText;
    string resultText;

    //대문자로 변환하고 pairSecond를 pairFirst로 대체
    for(int i = 0; i < beforeEncryptionInput.length(); ++i) {
        char ch = beforeEncryptionInput[i];
        if(isalpha(ch)) {
            ch = toupper(ch);
            if(ch == toupper(mPair[2])) { 
                ch = toupper(mPair[0]);
            }
            preparedInput += ch;
        }
    }

    //두 글자씩 끊고, 동일한 글자가 나오면 X 출력
    int i = 0;
    while(i < preparedInput.length()) {    // 두 문자씩 끊기.
        char first = preparedInput[i];
        char second = 'X';    // 기본으로 두 번째 문자는 X로 처리

        if(i + 1 < preparedInput.length()) {    //마지막 문자가 아닐 경우
            second = preparedInput[i+1];
            if(first == second) {
                beforeText += first;
                beforeText += 'X'; // 항상 대문자 'X' 추가
                i += 1;         //second를 first로 두기 위함
            }
            else {
                beforeText += first;
                beforeText += second;
                i += 2;         //first, second 둘다 처리 완료.
            }
        }

        else {      //마지막 문자일 경우
            beforeText += first;
            beforeText += 'X'; // 항상 대문자 'X' 추가
            i += 1;
        }
    }

    //암호화 시작~
    for(int j = 0; j < beforeText.length(); j += 2) {
        char a = beforeText[j];
        char b = beforeText[j + 1];

        int rowA, colA, rowB, colB;
        findPosition(a, rowA, colA);
        findPosition(b, rowB, colB);

        //1차원 배열을 2차원 배열로 봤을 때 row*5 + col로 계산. => row(0일때 5개, 1일때 5개)
        //%의 쓰임 -> 행과 열의 마지막에 도달했을 때 첫 행, 첫 열로 이동하기 위함.
        if(rowA == rowB){
            // 같은 행에 있는 경우 오른쪽으로 한 칸 이동
            
            char textA = mTable[rowA * 5 + (colA + 1) % 5];
            char textB = mTable[rowB * 5 + (colB + 1) % 5];
            resultText += textA;
            resultText += textB;
        }

        else if(colA == colB){
            // 같은 열에 있는 경우 아래로 한 칸 이동
            char textA = mTable[((rowA + 1) % 5) * 5 + colA];
            char textB = mTable[((rowB + 1) % 5) * 5 + colB];
            resultText += textA;
            resultText += textB;
        }

        else{
            // 다른 행과 열에 있는 경우 행과 열을 교환
            char textA = mTable[rowA * 5 + colB];
            char textB = mTable[rowB * 5 + colA];
            resultText += textA;
            resultText += textB;
        }

        resultText += ' '; // 두 글자 후에 공백 추가
    }

    return resultText;
}
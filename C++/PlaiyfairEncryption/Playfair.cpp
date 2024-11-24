#include <iostream>
#include <string>
using namespace std;

class Playfair {
    char mTable[25];
    string mPair;
    string mKey;
public:
    Playfair(string mKey, string mPair);
    void makeTable();
    void showTable();
	string makeEncryption(string mEncryption);
    string removeDuplicates(string input);
	void findPosition(char ch, int &row, int &col);
};

Playfair::Playfair(string mKey, string mPair) {
    this->mKey = removeDuplicates(mKey);
    this->mPair = mPair;
}

string Playfair::removeDuplicates(string input){
    string result;

    for (int i = 0; i < input.length(); ++i) {
        char ch = toupper(input[i]); // 대문자로 변환
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
    char pairFirst = toupper(mPair[0]);   // 예: 'I'
    char pairSecond = toupper(mPair[2]);  // 예: 'J'

    int tableIndex = 0;

    // 1. 키의 문자들을 테이블에 추가, 쌍의 두 번째 문자를 건너뜀
    for (int i = 0; i < key.length() && tableIndex < 25; ++i) {
        char ch = toupper(key[i]);

        if(ch == pairSecond) continue; // pairSecond ('J')는 건너뜀

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

    // 2. 알파벳을 순회하며 테이블에 추가, 쌍의 두 번째 문자를 건너뜀
    for (int i = 0; i < alphabet.length() && tableIndex < 25; ++i) {
        char ch = toupper(alphabet[i]);

        if(ch == pairSecond) continue; // pairSecond ('J')는 건너뜀

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

string Playfair::makeEncryption(string mEncryption){
	string upperProcessed;
    string paired;
    string resultText;

    // 1. 대문자로 변환하고 pairSecond를 pairFirst로 대체
    for(int i = 0; i < mEncryption.length(); ++i) {
        char ch = mEncryption[i];
        if(isalpha(ch)) {
            ch = toupper(ch);
            if(ch == toupper(mPair[2])) { // pairSecond ('J')를 pairFirst ('I')로 대체
                ch = toupper(mPair[0]);    // 'I'
            }
            upperProcessed += ch;
        }
    }

    // 2. 페어링: 두 글자씩 페어로 나누기, 동일한 글자가 연속되면 'X' 삽입
    int i = 0;
    while(i < upperProcessed.length()) {    // 두 문자씩 페어로 처리
        char first = upperProcessed[i];
        char second = 'X';    // 기본으로 두 번째 문자는 X로 처리

        if(i + 1 < upperProcessed.length()) {
            second = upperProcessed[i+1];
            if(first == second) {
                paired += first;
                paired += 'X'; // 항상 대문자 'X' 추가
                i += 1;
            }
            else {
                paired += first;
                paired += second;
                i += 2;
            }
        }
        else {
            paired += first;
            paired += 'X'; // 항상 대문자 'X' 추가
            i += 1;
        }
    }

    // 3. 암호화: 각 페어에 대해 Playfair 암호 규칙 적용
    for(int j = 0; j < paired.length(); j += 2) {
        char a = paired[j];
        char b = paired[j + 1];

        int rowA, colA, rowB, colB;
        findPosition(a, rowA, colA);
        findPosition(b, rowB, colB);

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

int main() {
    Playfair pfair("watermelon", "i/j");
    pfair.makeTable();
    pfair.showTable();
	cout << "암호화된 문장 : " << pfair.makeEncryption("lemonstrawberryappleiuice") << endl;

    return 0;
}
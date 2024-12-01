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
	string makeEncryption(string beforeEncryptionInput);
    string removeDuplicates(string input);
	void findPosition(char ch, int &row, int &col);
};
#include <iostream>
#include <string>
#include "Playfair.h"
using namespace std;

int main() {
    Playfair pfair("assasinator", "q/z");
    pfair.makeTable();
    pfair.showTable();
	cout << "암호화된 문장 : " << pfair.makeEncryption("becarefull") << endl;

    return 0;
}
#include <iostream>
#include <string>
#include "Playfair.h"
using namespace std;

int main() {
    Playfair pfair("watermelon", "i/j");
    pfair.makeTable();
    pfair.showTable();
	cout << "암호화된 문장 : " << pfair.makeEncryption("lemonstrawberryappleiuice") << endl;

    return 0;
}
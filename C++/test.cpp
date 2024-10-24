#include <iostream>
using namespace std;

int main() {
    int *p;

    p = new int;
    if(!p) {
        cout << "메모리 X";
        return 0;
    }

    *p = 5;
    int n = *p;
    cout << "*p = " << *p << 
}
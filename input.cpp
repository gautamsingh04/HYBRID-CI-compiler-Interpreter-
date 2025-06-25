#include <iostream>
using namespace std;

int main() {
    // Sample program for interpreter
    int x = 0;
    int y = 5;
    while (x < y) {
        if (x % 2 == 0) {
            y = y - 1;
        } else {
            y = y - 2;
        }
        x = x + 1;
    }
    return 0;
}

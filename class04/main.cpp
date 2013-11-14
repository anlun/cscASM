#include <iostream>
#include <cstdlib>
#include <iomanip>

using namespace std;

long getNextLong(int base) {
    string line;
    getline(cin, line);
    return strtol(line.c_str(), 0, base);
}

long long getNextLongLong(int base) {
    string line;
    getline(cin, line);
    return strtoll(line.c_str(), 0, base);
}

long logicAddress;
long selector;

long long* gdt = 0;
long gdtLineNumber;

long long* ldt = 0;
long ldtLineNumber;

void read() {
    string line;

    logicAddress  = getNextLong(16);
    selector      = getNextLong(16);

    gdtLineNumber = getNextLong(10);
    gdt = new long long[gdtLineNumber];
    for (long i = 0; i < gdtLineNumber; i++) {
        long long gdtLine = getNextLongLong(16);
        gdt[i] = gdtLine;
    }

    ldtLineNumber = getNextLong(10);
    ldt = new long long[ldtLineNumber];
    for (long i = 0; i < ldtLineNumber; i++) {
        long long ldtLine = getNextLongLong(16);
        ldt[i] = ldtLine;
    }
}

void clear() {
    if (gdt != 0) {
        delete [] gdt;
    }
    if (ldt != 0) {
        delete [] ldt;
    }
}

bool calculateAddress() {
    long long* table = gdt;
    long tableSize = gdtLineNumber;
    bool isLDT = selector & 0x0004;
    if (isLDT) {
        table = ldt;
        tableSize = ldtLineNumber;
    }

    long lineNumber = selector >> 3;
    if (lineNumber >= tableSize) {
        return false;
    }
    if ((lineNumber == 0) && !isLDT) {
        return false;
    }

    long long descriptor = table[lineNumber];
    long bigPart         = (long) descriptor >> 32;
    long littlePart      = descriptor & 0xFFFFFFFF;

    long base           = (littlePart >> 16) + ((bigPart & 0x000000FF) << 16) + (bigPart & 0xFF000000);

    long segLimit       = ((bigPart & 0x00FF0000) >> 16) + (littlePart & 0xFFFF);
    bool granularity    = (bigPart & 0x00800000) != 0;
    long checkingOffset = logicAddress;

    if (granularity) {
        checkingOffset >>= 12;
    }
    if (checkingOffset > segLimit) {
        return false;
    }

    cout << setw(8)
         << setfill('0')
         << hex
         << base + logicAddress << endl;
    return true;
}

int main() {
    read();
    if (!calculateAddress()) {
        cout << "INVALID" << endl;
    }
    clear();

	return 0;
}

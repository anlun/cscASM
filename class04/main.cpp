#include <iostream>
#include <cstdlib>
#include <iomanip>

using namespace std;

unsigned long getNextLong(int base) {
    string line;
    getline(cin, line);
    return strtoul(line.c_str(), 0, base);
}

unsigned long long getNextLongLong(int base) {
    string line;
    getline(cin, line);
    return strtoull(line.c_str(), 0, base);
}

long logicAddress;
long selector;

unsigned long long* gdt = 0;
long gdtLineNumber;

unsigned long long* ldt = 0;
long ldtLineNumber;

unsigned long long* pdt = 0;
long pdtLineNumber;

unsigned long long* ptt = 0;
long pttLineNumber;

void readTable(unsigned long long* &table, long &lineNumber) {
    lineNumber = getNextLong(10);
    table = new unsigned long long[lineNumber];
    for (long i = 0; i < lineNumber; i++) {
        unsigned long long line = getNextLongLong(16);
        table[i] = line;
    }
}

void read() {
    logicAddress  = getNextLong(16);
    selector      = getNextLong(16);

    readTable(gdt, gdtLineNumber);
    readTable(ldt, ldtLineNumber);
    readTable(pdt, pdtLineNumber);
    readTable(ptt, pttLineNumber);
}

void clear() {
    if (gdt != 0) {
        delete [] gdt;
    }
    if (ldt != 0) {
        delete [] ldt;
    }
    if (pdt != 0) {
        delete [] pdt;
    }
    if (ptt != 0) {
        delete [] ptt;
    }
}

bool calculateLinearAddress(long &address) {
    unsigned long long* table = gdt;
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

    unsigned long long descriptor = table[lineNumber];
    long bigPart        = (long) descriptor >> 32;
    long littlePart     = descriptor & 0xFFFFFFFF;

    long base           = (littlePart >> 16) + ((bigPart & 0x000000FF) << 16) + (bigPart & 0xFF000000);
    long segLimit       = (bigPart & 0x000F0000) + (littlePart & 0x0000FFFF);
    bool granularity    = (bigPart & 0x00800000) != 0;
    long checkingOffset = logicAddress;

    if (granularity) {
        checkingOffset >>= 12;
    }

    if (checkingOffset > segLimit) {
        return false;
    }

    address = base + logicAddress;
    return true;
}

bool calculatePhysicalAddress(long linearAddress) {
    long pageDirectoryNumber = (linearAddress & 0xFFC00000) >> 22;

    unsigned long long pde = pdt[pageDirectoryNumber];
    long pdeLastBit = pde & 0x1;
    if (!pdeLastBit) {
        return false;
    }
    long pdePS = (pde & 0x80) >> 7;
    if (pdePS) {
        unsigned long long offset  = linearAddress & 0x03FFFFF;
        unsigned long long bit3932 = (pde & 0x1FE000) >> 13;
        unsigned long long bit3122 = (pde & 0xFFC00000);

        unsigned long long physicalAddress = offset + (bit3932 << 32) + bit3122;
        cout << setw(8)
             << setfill('0')
             << hex
             << physicalAddress;
        cout << ":1" << endl;
        return true;
    }

    // Reading from Page table
    long pageTableNumber = (linearAddress & 0x3FF000) >> 12;
    unsigned long long pte = ptt[pageTableNumber];
    long pteLastBit = pte & 0x1;

    unsigned long long pageFrameAddress = pte & 0xFFFFF000;
    unsigned long long offset = linearAddress & 0x0FFF;

    unsigned long long physicalAddress = offset + pageFrameAddress;
    cout << setw(8)
         << setfill('0')
         << hex
         << physicalAddress;
    cout << ":" << pteLastBit << endl;
    return true;
}

int main() {
    read();
    long linearAddress;
    if (!calculateLinearAddress(linearAddress)) {
        cout << "INVALID" << endl;
        clear();
        return 0;
    }

    if (!calculatePhysicalAddress(linearAddress)) {
        cout << "INVALID" << endl;
        clear();
        return 0;
    }

    clear();

    return 0;
}

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include "MCB.h"
#include "MemoryArea.h"

using namespace std;

static MemoryArea* memoryArea = NULL;

void freeMemoryArea() {
    delete memoryArea;
    memoryArea = NULL;
}

void initMemoryArea(short n) {
    if (memoryArea != NULL) {
        freeMemoryArea();
    }
    memoryArea = new MemoryArea(n);
}

void* my_alloc(size_t size) {
    void* ptr = memoryArea->alloc((short) size);
    if (ptr == NULL) {
        cout << "-" << endl;
        return ptr;
    }
    ptr += MCB::sizeToSave();
    cout << "+ " << memoryArea->ptrToOffset(ptr) << endl;
    return ptr;
}

void my_delete(void* ptr) {
    ptr -= MCB::sizeToSave();
    if (memoryArea->free(ptr)) {
        cout << "+" << endl;
    } else {
        cout << "-" << endl;
    }
}

void my_delete(short offset) {
    offset -= MCB::sizeToSave();
    memoryArea->free(offset);
}

void proceedCmdFile(const string& fileName, bool flag) {
    ifstream in(fileName.c_str());

    string line;
    getline(in, line);

    short heapSize;
    istringstream(line) >> heapSize;

    initMemoryArea(heapSize);

    while (getline(in, line)) {
        if (flag) {
            cout << line << ":\n\t";
        }

        if (line[0] == 'A') {
            short size;
            sscanf(line.c_str(), "ALLOC %hd", &size);
            my_alloc(size);
            continue;
        }

        if (line[0] == 'F') {
            short offset;
            sscanf(line.c_str(), "FREE %hd", &offset);
            my_delete(offset);
            continue;
        }

        if (line[0] == 'I') {
            memoryArea->printInfo();
            continue;
        }

        if (line[0] == 'M') {
            memoryArea->printMap();
            continue;
        }
    }

    freeMemoryArea();
    in.close();
}

int main(int argc, char* argv[]) {
    cout << "TEST 1\n=========" << endl;
    proceedCmdFile("test1", false);
    cout << endl;
    cout << "TEST 2\n=========" << endl;
    proceedCmdFile("test2", false);

    cout << endl;
    cout << "TEST 1\n=========" << endl;
    proceedCmdFile("test1", true);
    cout << endl;
    cout << "TEST 2\n=========" << endl;
    proceedCmdFile("test2", true);
    return 0;
}

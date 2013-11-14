#pragma once

#include <string>
#include "MCB.h"

class MemoryArea {
public:
    MemoryArea(short size);

    void* alloc      (short size) const;
    short ptrToOffset(void*  ptr) const;
    bool  free       (void*  ptr) const;

    bool free(short offset) const;
    ~MemoryArea();

    void printInfo() const;
    void printMap () const;

private:
    MCB  getRootMCB() const;
    MCB   getMCB(short offset, short prevMCB) const;
    void saveMCB(MCB const &mcb) const;

    bool  freeMCB(MCB const  &mcb, const MCB* const prevMCB) const;
    MCB  mergeMCB(MCB const &left, MCB const &right) const;

    static std::string showMCB(MCB const &mcb);

    char*       mArea;
    short const mSize;
};

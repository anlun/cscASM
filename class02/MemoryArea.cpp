#include <cstddef>
#include <cassert>
#include <sstream>
#include <iostream>
#include "MemoryArea.h"

using namespace std;

MemoryArea::MemoryArea(short size): mSize(size) {
    assert(size > 0);
    mArea = new char[mSize];
    MCB rootMCB(-(size - MCB::sizeToSave()), 0, -1, mSize);
    saveMCB(rootMCB);
}

void* MemoryArea::alloc(short size) const {
    assert(size > 0);

    short  curOffset =  0;
    short prevOffset = -1;

    while (true) {
        MCB const curMCB = getMCB(curOffset, prevOffset);

        if (curMCB.isFreeBlock() && curMCB.getSize() >= size) {                       
            if (curMCB.getSize() <= size + MCB::sizeToSave()) {
                // We have no enough memory to save 2 blocks.
                MCB newMCB(curMCB.getSize(), curOffset, curMCB.getPrev(), mSize);
                saveMCB(newMCB);
                return mArea + curOffset;
            }

            //We habe enough memory to save 2 blocks.
            MCB newMCB(size, curOffset, curMCB.getPrev(), mSize);

            short const freeBlockSize = curMCB.getSize() - MCB::sizeToSave() - size;
            MCB newFreeMCB(-freeBlockSize, newMCB.getNext(), curOffset, mSize);

            saveMCB(newMCB);
            saveMCB(newFreeMCB);
            return mArea + curOffset;
        }

        if (!curMCB.hasNext()) {
            break;
        }

        prevOffset = curOffset;
         curOffset = curMCB.getNext();
    }
    return NULL;
}

short MemoryArea::ptrToOffset(void* ptr) const {
    assert((long) ptr >= (long) mArea);
    return (long) ptr -  (long) mArea;
}

bool MemoryArea::free(void* ptr) const {
    short offset = (long) ptr - (long) mArea;
    assert(offset > 0);
    return free(offset);
}

MCB MemoryArea::mergeMCB(const MCB &left, const MCB &right) const {
    short mergedSize = left.getSize() + right.getSize() + MCB::sizeToSave();
    MCB merged(-mergedSize, left.getPosition(), left.getPrev(), mSize);
    return merged;
}

bool MemoryArea::freeMCB(const MCB &mcb, const MCB * const prevMCB) const {
    if (mcb.isFreeBlock()) {
        return false;
    }

    MCB newMCB = mcb.getFreeMCB();
    if (prevMCB != NULL && prevMCB->isFreeBlock()) {
        newMCB = mergeMCB(*prevMCB, mcb);
    }

    if (mcb.hasNext()) {
        MCB nextMCB = getMCB(mcb.getNext(), mcb.getPosition());
        if (nextMCB.isFreeBlock()) {
            newMCB = mergeMCB(newMCB, nextMCB);
        }
    }

    saveMCB(newMCB);

    return true;
}

bool MemoryArea::free(short offset) const {
    assert(offset >= 0);

    MCB   curMCB = getMCB(0, -1);
    MCB* prevMCB = NULL;

    while (true) {
        if (curMCB.getPosition() > offset) {
            break;
        }

        if (curMCB.getPosition() == offset) {
            return freeMCB(curMCB, prevMCB);
        }

        // Iteration variable update.
        if (!curMCB.hasNext()) {
            break;
        }

        prevMCB = &curMCB;
         curMCB = getMCB(curMCB.getNext(), curMCB.getPosition());
    }

    return false;
}

MemoryArea::~MemoryArea() {
    delete [] mArea;
}

MCB MemoryArea::getRootMCB() const {
    return getMCB(0, -1);
}

MCB MemoryArea::getMCB(short offset, short prevMCB) const {
    char* place = mArea + offset;

    short mcbSize = (place[0] & 0x00FF) + ((place[1] & 0x00FF) << 8);
    // short prevMCB = place[2] + place[3] << 8;

    return MCB(mcbSize, offset, prevMCB, mSize);
}

void MemoryArea::saveMCB(MCB const &mcb) const {
    char* place = mArea + mcb.getPosition();
    short mcbSize = mcb.getRealSize();

    place[0] =  mcbSize & (0x00FF);
    place[1] = (mcbSize & (0xFF00)) >> 8;

    // short prevMCB = mcb.getPrev();
    // place[2] =  prevMCB & (0x00FF);
    // place[3] = (prevMCB & (0xFF00)) >> 8;
}

void MemoryArea::printInfo() const {
    MCB   curMCB = getMCB(0, -1);

    short    allocatedBlockCount = 0; // N1
    short    allocatedMemorySize = 0; // N2
    short         freeMemorySize = 0; // N3
    short maxFreetoAllocateBlock = 0; // N4

    while (true) {
        if (curMCB.isFreeBlock()) {
            short const curFreeToAllocateBlock = curMCB.getSize() + MCB::sizeToSave();
            if (curFreeToAllocateBlock > maxFreetoAllocateBlock) {
                maxFreetoAllocateBlock = curFreeToAllocateBlock;
            }
            freeMemorySize += curFreeToAllocateBlock;
        } else {
            allocatedBlockCount++;
            allocatedMemorySize += curMCB.getSize();
        }

        // Iteration variable update.
        if (!curMCB.hasNext()) {
            break;
        }

        curMCB = getMCB(curMCB.getNext(), curMCB.getPosition());
    }

    cout <<    allocatedBlockCount << ' '
         <<    allocatedMemorySize << ' '
         <<         freeMemorySize << ' '
         << maxFreetoAllocateBlock << ' '
         << endl;
}

string MemoryArea::showMCB(MCB const &mcb) {
    char symbol = 'u';
    if (mcb.isFreeBlock()) {
        symbol = 'f';
    }

    stringstream block;
    for (int i = 0; i < mcb.getSize(); i++) {
        block << symbol;
    }

    return "mm" + block.str();
}

void MemoryArea::printMap() const {
    MCB curMCB = getMCB(0, -1);

    while (true) {
        cout << MemoryArea::showMCB(curMCB);

        // Iteration variable update.
        if (!curMCB.hasNext()) {
            break;
        }

        curMCB = getMCB(curMCB.getNext(), curMCB.getPosition());
    }

    cout << endl;
}

#pragma once

class MCB {
public:
    MCB(short size, short position, short prev, short heapSize);

    short getSize    () const;
    short getRealSize() const;
    short getPrev    () const;
    short getPosition() const;
    short getNext    () const;

    bool isFreeBlock() const;
    bool hasNext    () const;

    MCB getFreeMCB  () const;

    static short sizeToSave();

private:
    short mSize;
    short mPosition;
    short mPrev;
    short mHeapSize;
};

#include <cstdlib>
#include "MCB.h"

MCB::MCB(short size, short position, short prev, short heapSize)
: mSize(size)
, mPosition(position)
, mPrev(prev)
, mHeapSize(heapSize) {
}

short MCB::getSize() const {
    return abs(mSize);
}

short MCB::getRealSize() const {
    return mSize;
}

short MCB::getPrev() const {
    return mPrev;
}

short MCB::getPosition() const {
    return mPosition;
}

short MCB::getNext() const {
    return mPosition + MCB::sizeToSave() + getSize();
}

bool MCB::isFreeBlock() const {
    return mSize < 0;
}

bool MCB::hasNext() const {
    return getNext() < mHeapSize;
}

MCB MCB::getFreeMCB() const {
    return MCB(-getSize(), mPosition, mPrev, mHeapSize);
}

short MCB::sizeToSave() {
    return sizeof(mSize);//+ sizeof(mPrev);
}

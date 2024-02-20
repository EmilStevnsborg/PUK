#include "Buffer.h"

Buffer::Buffer(int lines, int bytesLine) {
    this->lines = lines;
    this->bytesLine = bytesLine;
    this->bytesAllocated = lines*bytesLine;
    this->memory = (byte*) malloc(bytesAllocated*sizeof(byte));
}

int Buffer::GetBytesAllocted() {
    return bytesAllocated;
}

void Buffer::FreeMemory() {
    free(this->memory);
}
#include "Buffer.h"


// circular buffer
Buffer::Buffer(int channels, int rows, int cols, int lines, bool hasExtraMemory)
        : lineMemoryMap(lines)
{
    // image
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;

    // buffer
    this->lines = lines;
    this->bytesLine = cols*channels;

    this->bytesAllocated = lines*bytesLine;
    this->memory = (byte*) malloc(bytesAllocated*sizeof(byte));

    this->hasExtraMemory = hasExtraMemory;
    if (hasExtraMemory) {
        this->extraMemory = (byte*) malloc(bytesAllocated*sizeof(byte));
    } else {
        this->extraMemory = nullptr;
    }
    
    // init lineMemoryMap to -1 (empty buffer)
    fill(this->lineMemoryMap.begin(), this->lineMemoryMap.end(), -1);
    
    this->lineInserts = 0;
}

void Buffer::LineInserted() {
    int newMemoryMapIdx = this->lineInserts % this->lines;
    lineMemoryMap[newMemoryMapIdx] = lineInserts;
    lineInserts += 1;
}

void Buffer::FreeMemory() {
    free(this->memory);
}
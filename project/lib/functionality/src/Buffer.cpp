#include "Buffer.h"


// circular buffer
Buffer::Buffer(int channels, int rows, int cols, int lines, 
               bool hasExtraMemory, int byteScaleFactor)
        : lineMemoryMap(lines)
{
    // image
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;

    // scale factor for how many extra bytes we might need to store
    this->byteScaleFactor = byteScaleFactor;

    // buffer
    this->lines = lines;
    this->bytesLine = cols*channels*byteScaleFactor;

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

// can only be called for line already in buffer ie. inputBuffer
int Buffer::LineMemoryIndex(int line) {
    if (line > this->lineInserts || this->lineInserts - line > this->lines) {
        // line is not in buffer
        // throw error
    }
    return line % this->lines * bytesLine;
}

void Buffer::FreeMemory() {
    free(this->memory);
}
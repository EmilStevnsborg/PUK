#include "Buffer.h"

// circular buffer
Buffer::Buffer(int channels, int rows, int cols, int lines, 
               bool hasExtraMemory, bool storesBytes)
{
    // image
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;

    // buffer
    this->lines = lines;
    this->lineSize = cols*channels;
    this->elementsAllocated = lines*this->lineSize;

    this->storesBytes = storesBytes;

    if (storesBytes) {
        this->memoryUINT8 = (byte*) malloc(this->elementsAllocated*sizeof(byte));
        this->memoryUINT16 = nullptr;
    } else {
        this->memoryUINT16 = (uint16_t*) malloc(this->elementsAllocated*sizeof(uint16_t));
        this->memoryUINT8 = nullptr;
    }

    this->hasExtraMemory = hasExtraMemory;
    if (hasExtraMemory) {
        this->extraMemory = (byte*) malloc(this->elementsAllocated*sizeof(byte));
    } else {
        this->extraMemory = nullptr;
    }
    
    this->lineInserts = 0;
}

template <>
uint16_t* Buffer::Memory<uint16_t>() {return memoryUINT16;}

template <>
byte* Buffer::Memory<byte>() {return memoryUINT8;}

void Buffer::LineInserted() {
    lineInserts += 1;
}

// can only be called for line already in buffer ie. inputBuffer
int Buffer::LineMemoryIndex(int line) {
    if (line > this->lineInserts || this->lineInserts - line > this->lines) {
        // line is not in buffer
        // throw error
    }
    return line % this->lines * lineSize;
}

void Buffer::FreeMemory() {
    if (this->storesBytes) {
        free(this->memoryUINT8);
    } else {
        free(this->memoryUINT16);
    }
    if (this->hasExtraMemory) {free(this->extraMemory);}
}
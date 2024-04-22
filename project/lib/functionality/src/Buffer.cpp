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

    this->constructorMalloc = true;
    
    this->lineInserts = 0;
}

// Used for top level functions
Buffer::Buffer(int channels, int rows, int cols, 
               int lines, bool hasExtraMemory, byte* bytePtr) 
{
    // image
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;

    // buffer
    this->lines = lines;
    this->lineSize = cols*channels;
    this->elementsAllocated = lines*this->lineSize;

    this->memoryUINT8 = bytePtr;
    this->storesBytes = true;

    this->hasExtraMemory = hasExtraMemory;
    if (hasExtraMemory) {
        this->extraMemory = (byte*) malloc(this->elementsAllocated*sizeof(byte));
    } else {
        this->extraMemory = nullptr;
    }

    this->memoryUINT16 = nullptr;

    this->constructorMalloc = false;

    this->lineInserts = 0;
}
Buffer::Buffer(int channels, int rows, int cols, 
               int lines, bool hasExtraMemory, uint16_t* uint16Ptr) 
{
    // image
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;

    // buffer
    this->lines = lines;
    this->lineSize = cols*channels;
    this->elementsAllocated = lines*this->lineSize;

    this->memoryUINT16 = uint16Ptr;
    this->storesBytes = false;
    
    this->hasExtraMemory = hasExtraMemory;
    if (hasExtraMemory) {
        this->extraMemory = (byte*) malloc(this->elementsAllocated*sizeof(byte));
    } else {
        this->extraMemory = nullptr;
    }
    
    this->memoryUINT8 = nullptr;

    this->constructorMalloc = false;

    this->lineInserts = 0;
}

Buffer::~Buffer() {
    this->FreeMemory();
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
    if (this->constructorMalloc) {
        if (this->storesBytes) {
            free(this->memoryUINT8);
        } else {
            free(this->memoryUINT16);
        }
        if (this->hasExtraMemory) {free(this->extraMemory);}
    }
}
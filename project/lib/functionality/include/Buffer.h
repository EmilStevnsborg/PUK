#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <cstdio>

typedef uint8_t byte;

class Buffer {
    public:

        // image specifics
        int channels;
        int rows;
        int cols;

        // buffer specifics
        int lines;
        int lineSize;
        int elementsAllocated;

        // do we store main data in memoryUINT8
        bool storesBytes;
        
        byte* memoryUINT8;
        uint16_t* memoryUINT16;

        // extra memory that can be used for storing metadata
        byte* extraMemory;
        bool hasExtraMemory;

        // times lines have been inserted besides the intital lines
        int lineInserts;
        
        // circular buffer
        Buffer(int channels, int rows, int cols, int lines, 
               bool hasExtraMemory = false, bool storesBytes = true);

        template <typename MemoryType>
        MemoryType* Memory();
        
        void LineInserted();
        // can only be called for line already in buffer ie. inputBuffer
        int LineMemoryIndex(int line);
        void FreeMemory();
};

#endif // BUFFER_H
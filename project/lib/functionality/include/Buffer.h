#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stdlib.h>
#include <vector>

typedef uint8_t byte;

class Buffer {
    public:

        // image specifics
        int channels;
        int rows;
        int cols;

        // buffer specifics
        int lines;
        int bytesLine;
        int bytesAllocated;
        
        byte* memory;

        // functionality specifics

        // what lines from input are stored where in buffer
        std::vector<int> lineMemoryMap;
        // times lines have been inserted besides the intital lines
        int lineInserts;
        
        // circular buffer
        Buffer(int channels, int rows, int cols, int lines);

        void FreeMemory();
        void LineInserted();
};

#endif // BUFFER_H
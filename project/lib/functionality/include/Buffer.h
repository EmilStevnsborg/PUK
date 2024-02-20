#include <stdint.h>
#include <stdlib.h>

typedef uint8_t byte;

class Buffer {
    private:
        int lines;
        int bytesLine;
        int bytesAllocated;
    public:
        byte* memory; // size = (number of lines needed) * (bytes per line)
        Buffer(int lines, int bytesLine);

        int GetBytesAllocted();
        void FreeMemory();
};
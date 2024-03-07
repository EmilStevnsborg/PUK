#ifndef LAYER_H
#define LAYER_H

#include "Buffer.h"

typedef uint8_t byte;

// Abstract class for a functionality layer
class Layer {
    public:
        Buffer inputBuffer;

        // Base constructor
        Layer() : 
            inputBuffer(0, 0, 0, 0) {}
        
        virtual void Stream(Buffer* outputBuffer, int line) = 0;
        virtual Buffer* InputBuffer() = 0;
        virtual int PadHeight() = 0;
        virtual ~Layer() {}
};
#endif // LAYER_H
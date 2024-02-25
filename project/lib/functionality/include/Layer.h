#ifndef LAYER_H
#define LAYER_H

#include "Buffer.h"

typedef uint8_t byte;

// Abstract class for a functionality layer
class Layer {
    private:
        // image specifics
        int inputChannels;
        int inputRows;
        int inputCols;
    public:
        Buffer inputBuffer;
        // Constructor with parameters
        Layer(int inputChannels, int inputRows, int inputCols) : 
            inputChannels(inputChannels), 
            inputRows(inputRows), 
            inputCols(inputCols), 
            inputBuffer(0, 0, 0, 0) {}
        virtual void Stream(Buffer* outputBuffer, int line) = 0;
        virtual ~Layer() {}
};

#endif // LAYER_H
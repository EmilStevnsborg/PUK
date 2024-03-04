#ifndef NonMaxSuppressionLayer_H
#define NonMaxSuppressionLayer_H

#include "Layer.h"
#include "CVfunctions.h"

class NonMaxSuppressionLayer : public Layer {
    private:
        // image specifics
        int inputChannels;
        int inputRows;
        int inputCols;
    public:
        Buffer inputBuffer;

        int kernelHeight;
        int kernelWidth;
        
        int padHeight;
        int padWidth;

        byte lowThreshold;
        byte highThreshold;

        NonMaxSuppressionLayer(int inputChannels, 
                               int inputRows, 
                               int inputCols,
                               int kernelHeight, 
                               int kernelWidth,
                               byte lowThreshold,
                               byte highThreshold);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}
        int PadHeight() {return padHeight;}

        ~NonMaxSuppressionLayer() {};
};

#endif // NonMaxSuppressionLayer_H
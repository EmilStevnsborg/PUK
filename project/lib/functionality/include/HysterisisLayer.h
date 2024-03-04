#ifndef HysterisisLayer_H
#define HysterisisLayer_H

#include "Layer.h"
#include "CVfunctions.h"

class HysterisisLayer : public Layer {
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

        HysterisisLayer(int inputChannels, 
                        int inputRows, 
                        int inputCols,
                        int kernelHeight, int kernelWidth);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}
        int PadHeight() {return padHeight;}

        ~HysterisisLayer() {};
};

#endif // HysterisisLayer_H
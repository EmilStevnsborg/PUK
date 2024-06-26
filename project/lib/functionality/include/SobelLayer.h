#ifndef SobelLayer_H
#define SobelLayer_H

#include "Layer.h"
#include "CVfunctions.h"

class SobelLayer : public Layer {
    public:
        Buffer inputBuffer;

        int kernelHeight;
        int kernelWidth;

        std::vector<std::vector<float>> kernelX;
        std::vector<std::vector<float>> kernelY;

        int padHeight;
        int padWidth;

        SobelLayer(int inputChannels, 
                   int inputRows, 
                   int inputCols,
                   int kernelHeight, int kernelWidth,
                   int bufferLines);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}

        std::vector<int> NextLines(int streamingLine);

        ~SobelLayer() {};
};

#endif // SobelLayerLayer_H
#ifndef GrayScaleLayer_H
#define GrayScaleLayer_H

#include "Layer.h"
#include <cstdio>

class GrayScaleLayer : public Layer {
    public:
        Buffer inputBuffer;

        int kernelHeight;
        int kernelWidth;

        int padHeight;
        int padWidth;

        std::vector<float> intensityWeights;

        GrayScaleLayer(int inputChannels, 
                       int inputRows, 
                       int inputCols,
                       int bufferLines);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}

        std::vector<int> NextLines(int streamingLine);

        ~GrayScaleLayer() {};
};

#endif // GrayScaleLayer_H
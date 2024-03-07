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
                       int inputCols);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}
        int PadHeight() {return padHeight;}

        ~GrayScaleLayer() {};
};

#endif // GrayScaleLayer_H
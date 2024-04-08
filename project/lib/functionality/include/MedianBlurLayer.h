#ifndef MEDIANBLURLAYER_H
#define MEDIANBLURLAYER_H

#include "Layer.h"
#include "CVfunctions.h"
#include <vector>
#include <cmath>
#include <cstdio>
#include <iostream>

class MedianBlurLayer : public Layer {
    public:
        Buffer inputBuffer;
        
        int kernelHeight;
        int kernelWidth;

        std::vector<std::vector<float>> kernel;

        int padHeight;
        int padWidth;

        MedianBlurLayer(int inputChannels, 
                        int inputRows, int inputCols,
                        int kernelHeight, int kernelWidth,
                        int bufferLines);

        // compute output line and stream into outputMemory
        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}

        std::vector<int> NextLines(int streamingLine);

        void GetKernel();

        ~MedianBlurLayer() {};
};

#endif // MEDIANBLURLAYER_H
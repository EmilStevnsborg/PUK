#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "Layer.h"
#include "CVfunctions.h"
#include <vector>

class GaussianBlurLayer : public Layer {
    private:
        // image specifics
        int inputChannels;
        int inputRows;
        int inputCols;

        std::vector<std::vector<float>> kernel;
    public:
        int kernelHeight;
        int kernelWidth;
        int padHeight;
        int padWidth;
        int strideHeight;
        int strideWidth;

        Buffer inputBuffer;
        GaussianBlurLayer(int inputChannels, int inputRows, int inputCols);
        
        // compute output line and stream into outputMemory
        void Stream(Buffer* outputBuffer, int line);
        ~GaussianBlurLayer() {};
};

#endif // GAUSSIANBLUR_H
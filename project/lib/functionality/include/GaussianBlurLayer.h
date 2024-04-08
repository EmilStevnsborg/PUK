#ifndef GAUSSIANBLURLAYER_H
#define GAUSSIANBLURLAYER_H

#include "Layer.h"
#include "CVfunctions.h"
#include <vector>
#include <cmath>
#include <cstdio>
#include <iostream>

class GaussianBlurLayer : public Layer {
    public:
        Buffer inputBuffer;
        
        int kernelHeight;
        int kernelWidth;

        std::vector<std::vector<float>> kernel;

        int padHeight;
        int padWidth;
        
        double sigmaX;
        double sigmaY;

        GaussianBlurLayer(int inputChannels, int inputRows, int inputCols,
                          int kernelHeight, int kernelWidth, 
                          double sigmaX, double sigmaY,
                          int bufferLines);

        // compute output line and stream into outputMemory
        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}

        std::vector<int> NextLines(int streamingLine);

        void GetKernel();

        ~GaussianBlurLayer() {};
};

#endif // GAUSSIANBLURLAYER_H
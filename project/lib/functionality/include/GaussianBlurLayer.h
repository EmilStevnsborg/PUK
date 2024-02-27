#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "Layer.h"
#include "CVfunctions.h"
#include <vector>
#include <cmath>
#include <cstdio>
#include <iostream>

class GaussianBlurLayer : public Layer {
    private:
        // image specifics
        int inputChannels;
        int inputRows;
        int inputCols;
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
                          double sigmaX, double sigmaY);
        
        void GetKernel();

        // compute output line and stream into outputMemory
        void Stream(Buffer* outputBuffer, int line);
        ~GaussianBlurLayer() {};
};

#endif // GAUSSIANBLUR_H
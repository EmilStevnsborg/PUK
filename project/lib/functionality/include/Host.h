#ifndef HOST_H
#define HOST_H

#include "Buffer.h"
#include "CVfunctions.h"
#include "Camera.h"
#include "Layer.h"
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <cstdio>
#include "GaussianBlurLayer.h"
#include "NonMaxSuppressionLayer.h"
#include "SobelLayer.h"
#include "HysterisisLayer.h"
#include "GrayScaleLayer.h"
#include "MinMaxNormLayer.h"
#include "MedianBlurLayer.h"
#include <functional>
#include <memory>

typedef uint8_t byte;

// one camera, multiple functions (different buffering)
class Host {
    private:
    public:
        Camera* camSensor; // Dependency injection
        int channels;
        int rows;
        int cols;
        int bytesLine;
        
        Host();
        void Configure(Camera* cam);
        
        void MedianBlur(Buffer* outputBuffer, 
                        int kernelHeight, int kernelWidth);
        
        void GaussianBlur(Buffer* outputBuffer, 
                          int kernelHeight, int kernelWidth, 
                          double sigmaX, double sigmaY);
                          
        void CannyEdge(Buffer* outputBuffer, 
                       byte lowThreshold, byte highThreshold);

        void Sobel(Buffer* outputBuffer);

        void PopulateBuffers(std::vector<std::unique_ptr<Layer>>& layers,
                             Buffer* outputBuffer,
                             int currentLayerIdx, int line);

        void StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                               Buffer* outputBuffer);

        ~Host() {}
};

#endif // HOST_H
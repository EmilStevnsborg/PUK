#ifndef HOST_H
#define HOST_H

#include "Buffer.h"
#include "CVfunctions.h"
#include "Camera.h"
#include "GaussianBlurLayer.h"
#include "Layer.h"
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <cstdio>
#include "NonMaxSuppressionLayer.h"
#include "SobelLayer.h"
#include <functional>
#include <memory>

typedef uint8_t byte;

// one camera, multiple functions (different buffering)
class Host {
    private:
        int channels;
        int rows;
        int cols;
        int bytesLine;
        Camera* camSensor; // Dependency injection
    public:
        Host(Camera* cam, int channels, int rows, int cols);
        void CannyEdge(Buffer* outputBuffer);
        
        void GaussianBlur(Buffer* outputBuffer, 
                          int kernelHeight, int kernelWidth, 
                          double sigmaX, double sigmaY);

        void Sobel(Buffer* outputBuffer, 
                   int kernelHeight, int kernelWidth);

        void StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers, Buffer* outputBuffer);

        void PopulateBuffers(std::vector<std::unique_ptr<Layer>>& layers, Buffer* outputBuffer, 
                             int currentLayerIdx, int line);

        ~Host() {}
};

#endif // HOST_H
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
#include "compression.h"
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
        
        void MedianBlur(byte* output, 
                        int kernelHeight, int kernelWidth);
        
        void GaussianBlur(byte* output, 
                          int kernelHeight, int kernelWidth, 
                          double sigmaX, double sigmaY);
                          
        void CannyEdge(byte* output, 
                       byte lowThreshold, byte highThreshold);

        void Sobel(byte* output);

        void Encode(byte* output, std::string compressionType, std::string function);

        void PopulateBuffers(std::vector<std::unique_ptr<Layer>>& layers,
                             Buffer* bufferPtr,
                             int currentLayerIdx, int line);

        void StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                               Buffer* bufferPtr);
        
        void StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                               Compression* compression);

        void StreamingPipeLine(Compression* compression);

        ~Host() {}
};

#endif // HOST_H
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
                        int kernelHeight, int kernelWidth,
                        std::string compressionType);
        
        void MedianBlur(uint16_t* output, 
                        int kernelHeight, int kernelWidth);
        
        void GaussianBlur(byte* output, 
                          int kernelHeight, int kernelWidth, 
                          double sigmaX, double sigmaY, 
                          std::string compressionType);

        void GaussianBlur(byte* output, 
                          int kernelHeight, int kernelWidth, 
                          double sigmaX, double sigmaY);

        void Sobel(byte* output, std::string compressionType);
        
        void Sobel(uint16_t* output);

        void CannyEdge(byte* output, 
                       byte lowThreshold, byte highThreshold,
                       std::string compressionType);

        void CannyEdge(byte* output, 
                       byte lowThreshold, byte highThreshold);

        void QOIencode(byte* output);

        void PopulateBuffers(std::vector<std::unique_ptr<Layer>>& layers,
                             Buffer* bufferPtr,
                             int currentLayerIdx, int line);

        void StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                               Buffer* bufferPtr);
        
        void StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                               Compression* compressionPtr);

        void StreamingPipeLine(Compression* compressionPtr);

        ~Host() {}
};

#endif // HOST_H
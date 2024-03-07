#include "SobelLayer.h"

SobelLayer::SobelLayer(int inputChannels, 
                       int inputRows, 
                       int inputCols,
                       int kernelHeight, int kernelWidth)     
    : Layer(),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight, false, 1)
{
    this->kernelHeight = kernelHeight;
    this->kernelWidth = kernelWidth;

    // integer division is floored
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;


    // generalise these kernels
    this->kernelX = {{-1, 0, 1}, 
                     {-2, 0, 2}, 
                     {-1, 0, 1}};
    this->kernelY = {{1, 2, 1}, 
                     {0, 0, 0}, 
                     {-1, -2, -1}};
}

void SobelLayer::Stream(Buffer* outputBuffer, int line) {

    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);

    for (int j = 0; j < outputBuffer->cols; j++) {
        for (int c = 0; c < outputBuffer->channels; c++) {

            float Gx = 0;
            float Gy = 0;
            
            // anchor pixel of input (update when introducing stride)
            int ai = line;
            int aj = j;

            // Sobel uses REFLECT_101 for padding: gradients at borders are 0
            if (!((aj == 0 || aj == inputBuffer.cols-1) ||
                  (ai == 0 || ai == inputBuffer.rows-1))) { 
                
                float maxGradient = 4 * 255;

                Gx = MatMul<float>(&inputBuffer, kernelX, 
                                   kernelHeight, kernelWidth,
                                   ai, aj, c);

                if (Gx > 255) {Gx = 255;}
                else if (Gx < -255) {Gx = -255;}

                Gy = MatMul<float>(&inputBuffer, kernelY, 
                                   kernelHeight, kernelWidth,
                                   ai, aj, c);

                if (Gy > 255) {Gy = 255;}
                else if (Gy < -255) {Gy = -255;}
            
            }

            // (min,max) = (0,361) which means the scale is roughly 1.42
            float maxMagnitude = std::sqrt(255*255 + 255*255);
            
            float gradientMagnitude = std::sqrt(Gx*Gx + Gy*Gy);

            gradientMagnitude = (gradientMagnitude / maxMagnitude) * 255;

            // Convert to degrees
            float degrees = std::atan2(Gy, Gx) * (180/M_PI);
            if (degrees < 0) {degrees += 180;}

            // There are four directions {up, diagonal-up, horizontal, diagonal-down}
            byte angle;

            // up down
            if ((degrees >= 0 && degrees < 22.5) || (degrees >= 157.5)) {
                angle = 0;
            }
            // diagonal-right 
            else if (degrees >= 22.5 && degrees < 67.5) {
                angle = 1;
            } 
            // horizontal
            else if (degrees >= 67.5 && degrees < 112.5) {
                angle = 2;
            } 
            // diagonal-left
            else if (degrees >= 112.5 && degrees < 157.5) {
                angle = 3;
            }
            
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;

            outputBuffer->memory[outIdx] = (byte) (std::floor(gradientMagnitude));
            outputBuffer->extraMemory[outIdx] = angle;
        }
    }
}
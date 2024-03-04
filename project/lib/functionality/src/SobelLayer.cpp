#include "SobelLayer.h"

SobelLayer::SobelLayer(int inputChannels, 
                       int inputRows, 
                       int inputCols,
                       int kernelHeight, int kernelWidth)     
    : Layer(inputChannels, inputRows, inputCols),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight)
{
    this->inputChannels = inputChannels;
    this->inputRows = inputRows;
    this->inputCols = inputCols;

    this->kernelHeight = kernelHeight;
    this->kernelWidth = kernelWidth;

    // integer division is floored
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;

    this->kernelX = {{-1, 0, 1}, 
                     {-2, 0, 2}, 
                     {-1, 0, 1}};
    this->kernelY = {{1, 2, 1}, 
                     {0, 0, 0}, 
                     {-1, -2, -1}};
}

void SobelLayer::Stream(Buffer* outputBuffer, int line) {

    // Computation requires [line-padHeight,..., line+padHeight} from input
    int startLine = line-padHeight;

    // Where the line should be placed in the memory of the outputBuffer
    int outMemIdx = (outputBuffer->lineInserts % outputBuffer->lines) * 
                     outputBuffer->bytesLine;

    // Compute gradient and angle for each pixel
    for (int startCol = -padWidth; startCol < inputCols; startCol++) {
        for (int c = 0; c < inputBuffer.channels; c++) {
            
            float Gx = Convolution<float>(&inputBuffer, kernelX, 
                                          kernelHeight, kernelWidth,
                                          startLine, startCol, c,
                                          inputChannels, inputCols);

            float Gy = Convolution<float>(&inputBuffer, kernelY, 
                                          kernelHeight, kernelWidth,
                                          startLine, startCol, c,
                                          inputChannels, inputCols);
            
            float gradientMagnitude = std::sqrt(Gx*Gx + Gy*Gy);
            
            // Convert radians to degrees
            float degrees = std::atan2(Gy, Gx) * (180/M_PI);
            if (degrees < 0) {degrees += 180;}

            // There are four directions {up, diagonal-up, horizontal, diagonal-down}
            byte angle;

            // up
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
            outputBuffer->memory[outMemIdx+c] = (byte) (std::floor(gradientMagnitude));
            outputBuffer->extraMemory[outMemIdx+c] = (angle);
        }
        outMemIdx += outputBuffer->channels;
    }
}
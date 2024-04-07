#include "SobelLayer.h"

SobelLayer::SobelLayer(int inputChannels, 
                       int inputRows, 
                       int inputCols,
                       int kernelHeight, int kernelWidth,
                       int bufferLines)     
    : Layer(),
      inputBuffer(inputChannels, inputRows, inputCols, bufferLines, false, true)
{
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
    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);

    for (int j = 0; j < outputBuffer->cols; j++) {
        for (int c = 0; c < outputBuffer->channels; c++) {

            float Gx = 0;
            float Gy = 0;
        
            
            // anchor pixel of input (update when introducing stride)
            int ai = line;
            int aj = j;

            // Sobel uses REFLECT_101 for the border
            Gx = MatMul<float>(&inputBuffer, kernelX, 
                                kernelHeight, kernelWidth,
                                ai, aj, c);

            Gy = MatMul<float>(&inputBuffer, kernelY, 
                                kernelHeight, kernelWidth,
                                ai, aj, c);        

            // Convert to degrees
            float degrees = std::atan2(Gy, Gx) * (180/M_PI);
            if (degrees < 0) {degrees += 180;}
            
            float gradientMagnitude = std::sqrt(Gx*Gx + Gy*Gy);

            // An edge direction is normal to the angle
            byte angle;

            // up down angle
            if ((degrees >= 0 && degrees < 22.5) || (degrees >= 157.5)) {
                angle = 0;
            }
            // diagonal-right angle
            else if (degrees >= 22.5 && degrees < 67.5) {
                angle = 45;
            } 
            // horizontal angle
            else if (degrees >= 67.5 && degrees < 112.5) {
                angle = 90;
            } 
            // diagonal-left angle
            else if (degrees >= 112.5 && degrees < 157.5) {
                angle = 135;
            }
            
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;

            // store gradient magnitude
            outputBuffer->Memory<uint16_t>()[outIdx] = (uint16_t) (std::floor(gradientMagnitude));
            // store angle
            outputBuffer->extraMemory[outIdx] = angle;
        }
    }
}

// streaming line this layer, what lines does inputbuffer need to do that
std::vector<int> SobelLayer::NextLines(int streamingLine) {
    std::vector<int> nextLines;

    // if buffer is already full, then nothing
    if (inputBuffer.lineInserts == inputBuffer.rows) {
        return nextLines;
    }
    
    int startLine = inputBuffer.lineInserts;
    int endLine = streamingLine + padHeight;
    
    for (int nextLine = startLine; nextLine <= endLine; nextLine++) {
        if (nextLine < inputBuffer.rows) {
            nextLines.push_back(nextLine);
        }
    }
    return nextLines;
}
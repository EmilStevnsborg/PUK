#include "HysterisisLayer.h"

HysterisisLayer::HysterisisLayer(int inputChannels, 
                                 int inputRows, 
                                 int inputCols,
                                 int kernelHeight, 
                                 int kernelWidth)     
    : Layer(),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight, false, 1) 
{
    this->kernelHeight = kernelHeight;
    this->kernelWidth = kernelWidth;

    // integer division is floored
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;
}


void HysterisisLayer::Stream(Buffer* outputBuffer, int line) {

    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);

    for (int j = 0; j < outputBuffer->cols; j++) {
        for (int c = 0; c < outputBuffer->channels; c++) {
            
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;
            
            bool keep = Hysterisis(&inputBuffer, line, j, c);
            
            if (keep) {
                outputBuffer->memory[outIdx] = 255;
            } else {
                outputBuffer->memory[outIdx] = 0;
            }
        }
    }
}
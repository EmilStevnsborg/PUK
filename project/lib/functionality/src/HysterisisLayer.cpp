#include "HysterisisLayer.h"

HysterisisLayer::HysterisisLayer(int inputChannels, 
                                 int inputRows, 
                                 int inputCols,
                                 int kernelHeight, 
                                 int kernelWidth)     
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
}


void HysterisisLayer::Stream(Buffer* outputBuffer, int line) {

    // Where the line should be placed in the memory of the outputBuffer
    int outMemIdx = (line % outputBuffer->lines) * 
                     outputBuffer->bytesLine;

	// for each pixel compute hysterisis
    for (int j = 0; j < inputCols; j++) {
        for (int c = 0; c < inputChannels; c++) {
            bool keep = Hysterisis(&inputBuffer, c, line, j);
            if (keep) {
                outputBuffer->memory[outMemIdx+j*outputBuffer->channels+c] = 255;
            } else {
                outputBuffer->memory[outMemIdx+j*outputBuffer->channels+c] = 0;
            }
        }
    }
}
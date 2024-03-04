#include "HysterisisLayer.h"

HysterisisLayer::HysterisisLayer(int inputChannels, 
                                 int inputRows, 
                                 int inputCols,
                                 int kernelHeight, 
                                 int kernelWidth)     
    : Layer(inputChannels, inputRows, inputCols),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight) 
{
	// this is 1 always
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
        bool keep = Hysterisis(&inputBuffer, line, j, inputRows, inputCols);
        if (keep) {
            outputBuffer->memory[outMemIdx+j] = 255;
        } else {
            outputBuffer->memory[outMemIdx+j] = 0;
        }
    }
}
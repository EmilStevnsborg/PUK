#include "GrayScaleLayer.h"

GrayScaleLayer::GrayScaleLayer(int inputChannels, 
                               int inputRows, 
                               int inputCols)     
    : Layer(inputChannels, inputRows, inputCols),
      inputBuffer(inputChannels, inputRows, inputCols, 1) 
{

    this->inputChannels = inputChannels;
    this->inputRows = inputRows;
    this->inputCols = inputCols;

    this->kernelHeight = 1;
    this->kernelWidth = 1;

    // integer division is floored
    this->padHeight = 0;
    this->padWidth = 0;

    this->intensityWeights = {0.299, 0.587, 0.114};
}

void GrayScaleLayer::Stream(Buffer* outputBuffer, int line) {

    // Where the line should be placed in the memory of the outputBuffer
    int outMemIdx = (line % outputBuffer->lines) * 
                     outputBuffer->bytesLine;

    // starting line idx in memory
    int inMemIdx = (line % inputBuffer.lines)*inputBuffer.bytesLine;

	// for each pixel compute hysterisis
    for (int j = 0; j < inputCols; j++) {
        byte val = 0;
        for (int c = 0; c < inputChannels; c++) {
            int inputIdx = inMemIdx+j*inputBuffer.channels+c;
            float weight = intensityWeights[c];
            val += (byte) ((float) inputBuffer.memory[inputIdx] * weight);
        }
        outputBuffer->memory[outMemIdx+j] = val;
    }
}
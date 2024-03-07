#include "GrayScaleLayer.h"

GrayScaleLayer::GrayScaleLayer(int inputChannels, 
                               int inputRows, 
                               int inputCols)     
    : Layer(),
      inputBuffer(inputChannels, inputRows, inputCols, 1, false, 1) 
{
    this->kernelHeight = 1;
    this->kernelWidth = 1;

    // integer division is floored
    this->padHeight = 0;
    this->padWidth = 0;

    this->intensityWeights = {0.299, 0.587, 0.114};
}

void GrayScaleLayer::Stream(Buffer* outputBuffer, int line) {
    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);

    int inLineMemIdx = inputBuffer.LineMemoryIndex(line);

    for (int j = 0; j < inputBuffer.cols; j++) {
        byte val = 0;

        for (int c = 0; c < inputBuffer.channels; c++) {
            int inputIdx = (inLineMemIdx +
                            j*inputBuffer.channels + 
                            c);
            float weight = intensityWeights[c];
            val += (byte) ((float) inputBuffer.memory[inputIdx] * weight);
        }
        outputBuffer->memory[outLineMemIdx+j] = val;
    }
}
#include "GrayScaleLayer.h"

GrayScaleLayer::GrayScaleLayer(int inputChannels, 
                               int inputRows, 
                               int inputCols,
                               int bufferLines)     
    : Layer(),
      inputBuffer(inputChannels, inputRows, inputCols, bufferLines, false, true) 
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
            val += (byte) ((float) inputBuffer.Memory<byte>()[inputIdx] * weight);
        }
        outputBuffer->Memory<byte>()[outLineMemIdx+j] = val;
    }
}

// streaming line this layer, what lines does inputbuffer need to do that
std::vector<int> GrayScaleLayer::NextLines(int streamingLine) {
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
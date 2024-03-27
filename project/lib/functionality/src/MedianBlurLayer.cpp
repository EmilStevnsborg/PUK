#include "MedianBlurLayer.h"

MedianBlurLayer::MedianBlurLayer(int inputChannels, int inputRows, 
                                 int inputCols,
                                 int kernelHeight, int kernelWidth)
    : Layer(),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight, false, true)
{
    this->kernelHeight = kernelHeight;
    this->kernelWidth = kernelWidth;

    // integer division is floored
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;
    
    // kernel
    this->kernel = std::vector<std::vector<float>>(kernelHeight,
                                    std::vector<float>(kernelWidth, 1));
}

// stream a blurred line to output: necssarry lines already in buffer
void MedianBlurLayer::Stream(Buffer* outputBuffer, int line) {

    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);

    for (int j = 0; j < outputBuffer->cols; j++) {
        for (int c = 0; c < outputBuffer->channels; c++) {
            
            // anchor pixel of input (update when introducing stride)
            int ai = line;
            int aj = j;

            // all these parameters are related to input
            float sumProduct = MatMul<float>(&inputBuffer, kernel, 
                                             kernelHeight, kernelWidth,
                                             ai, aj, c);
            
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;

            outputBuffer->Memory<uint16_t>()[outIdx] = (uint16_t) (std::floor(sumProduct));    
        }
    }
}

// streaming line this layer, what lines does inputbuffer need to do that
std::vector<int> MedianBlurLayer::NextLines(int streamingLine) {
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
#include "HysterisisLayer.h"

HysterisisLayer::HysterisisLayer(int inputChannels, 
                                 int inputRows, 
                                 int inputCols,
                                 uint16_t lowThreshold,
                                 uint16_t highThreshold)     
    : Layer(),
      inputBuffer(inputChannels, inputRows, inputCols, 3, false, false) 
{
    this->lowThreshold = lowThreshold;
    this->highThreshold = highThreshold;
}


void HysterisisLayer::Stream(Buffer* outputBuffer, int line) {

    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);

    for (int j = 0; j < outputBuffer->cols; j++) {
        for (int c = 0; c < outputBuffer->channels; c++) {
            
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;
            
            if ((j == 0 || j == outputBuffer->cols-1) || 
                (line == 0 || line == outputBuffer->rows-1)) 
            {
                outputBuffer->Memory<byte>()[outIdx] = 0;
                continue;
            }
            
            bool keep = Hysterisis(&inputBuffer, line, j, c, lowThreshold, highThreshold);
            
            if (keep) {
                outputBuffer->Memory<byte>()[outIdx] = 255;
            } else {
                outputBuffer->Memory<byte>()[outIdx] = 0;
            }
        }
    }
}

// streaming line this layer, what lines does inputbuffer need to do that
std::vector<int> HysterisisLayer::NextLines(int streamingLine) {
    std::vector<int> nextLines;

    // if buffer is already full, then nothing
    if (inputBuffer.lineInserts == inputBuffer.rows) {
        return nextLines;
    }
    
    int startLine = inputBuffer.lineInserts;
    int endLine = streamingLine + 1;
    
    for (int nextLine = startLine; nextLine <= endLine; nextLine++) {    
        if (nextLine < inputBuffer.rows) {
            nextLines.push_back(nextLine);
        }
    }
    return nextLines;
}
#include "MinMaxNormLayer.h"

MinMaxNormLayer::MinMaxNormLayer(int inputChannels, 
                                 int inputRows, 
                                 int inputCols,
                                 int bufferLines)     
    : Layer(),
      // buffer stores 16 bit gradient magnitudes of whole image
      inputBuffer(inputChannels, inputRows, inputCols, bufferLines, true, false) 
{
    this->max = 0;
    this->min = 65535;
    this->minMaxIterations = 0;
}

// inputBuffer has extraMemory as well, where the gradient angle is
void MinMaxNormLayer::Stream(Buffer* outputBuffer, int line) {
    findMinMax(); 

    this->minMaxIterations += 1;

    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);
    
    // anchor line
    int inLineMemIdx = inputBuffer.LineMemoryIndex(line);

	// for each pixel min max normalise
    for (int j = 0; j < outputBuffer->cols; j++) {
        for (int c = 0; c < outputBuffer->channels; c++) {

            int inputIdx = inLineMemIdx + j*inputBuffer.channels + c;

            uint16_t gradientMagnitude = inputBuffer.Memory<uint16_t>()[inputIdx];

            byte val = (byte) std::floor(((float) (gradientMagnitude - min)) /
                                         ((float) (this->max - this->min)) * 
                                         255);
            
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;
            
            // store min-max normalized magnitude in output
            outputBuffer->Memory<byte>()[outIdx] = val;
        }
    }
}

void MinMaxNormLayer::findMinMax() {
    for (int line = 0; line < inputBuffer.lines; line++) {
        for (int j = 0; j < inputBuffer.cols; j++) {
            for (int c = 0; c < inputBuffer.channels; c++) {
                
                int inputIdx = (line*inputBuffer.lineSize + 
                                j*inputBuffer.channels + 
                                c);
                
                uint16_t gradientMagnitude = inputBuffer.Memory<uint16_t>()[inputIdx];

                if (gradientMagnitude > this->max) {
                    this->max = gradientMagnitude;
                }
                if (gradientMagnitude < this->min) {
                    this->min = gradientMagnitude;
                }
            }
        }
    }
}

// streaming line this layer, what lines does inputbuffer need to do that
std::vector<int> MinMaxNormLayer::NextLines(int streamingLine) {
    std::vector<int> nextLines;

    // if buffer is already full, then nothing
    if (inputBuffer.lineInserts == inputBuffer.rows) {
        return nextLines;
    }
    
    // we need as many lines as the buffer size

    int startLine = inputBuffer.lineInserts;
    int endLine;
    if ((inputBuffer.lineInserts < inputBuffer.lines) 
        && (streamingLine < inputBuffer.lines)) 
    {
        endLine = inputBuffer.lines-1;
    } else {
        endLine = streamingLine;
    }
    
    for (int nextLine = startLine; nextLine <= endLine; nextLine++) {
        if (nextLine < inputBuffer.rows) {
            nextLines.push_back(nextLine);
        }
    }
    return nextLines;
}
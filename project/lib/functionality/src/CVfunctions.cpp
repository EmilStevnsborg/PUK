#include "CVfunctions.h"

template<typename KernelType>
void Convolution(Buffer* inputBuffer, Buffer* outputBuffer,
                 int& outMemIdx,
                 std::vector<std::vector<KernelType>>& kernel,
                 int& kernelHeight, int& kernelWidth,
                 int& startLine, int& startCol, // padding embedded
                 int& channels, int& cols) {

    // kernel indexers
    int ki;
    int kj;

    int inputIdx;

    float w;
    float inputVal;

    for (int c = 0; c < channels; c++) {
        
        KernelType conv = 0;
        
        // lines used in matrixmult 
        for (int i = 0; i < inputBuffer->lines; i++) {

            // line is stored at index i in the memory
            int line = inputBuffer->lineMemoryMap[i];

            // kernel begins at startLine
            ki = line - startLine;
            
            // top padding and bottom padding
            if ((ki >= kernelHeight) || (line < startLine)) {
                continue;
            }

            // go through kernel
            for (int j = 0; j < kernelWidth; j++) {
                
                // left padding and right padding
                if ((startCol + j < 0) ||(startCol + j >= cols)) {
                    continue;
                }
                
                kj = j;

                w = kernel[ki][kj];

                inputIdx = i*(inputBuffer->bytesLine) + (startCol+j)*channels + c;
                
                inputVal = (float) inputBuffer->memory[inputIdx];
                conv += w * inputVal;
            }
        }
        int outIdx = outMemIdx+c;
        outputBuffer->memory[outIdx] = (byte) (std::floor(conv));
    }
}

template void Convolution(Buffer* inputBuffer, Buffer* outputBuffer,
                 int& outMemIdx,
                 std::vector<std::vector<float>>& kernel,
                 int& kernelHeight, int& kernelWidth,
                 int& startLine, int& startCol, // padding embedded
                 int& channels, int& cols);

template void Convolution(Buffer* inputBuffer, Buffer* outputBuffer,
                 int& outMemIdx,
                 std::vector<std::vector<byte>>& kernel,
                 int& kernelHeight, int& kernelWidth,
                 int& startLine, int& startCol, // padding embedded
                 int& channels, int& cols);
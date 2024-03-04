#include "CVfunctions.h"

template<typename KernelType>
float Convolution(Buffer* inputBuffer,
                  std::vector<std::vector<KernelType>>& kernel,
                  int& kernelHeight, int& kernelWidth,
                  int& startLine, int& startCol, int c, 
                  int& channels, int& cols) 
{
    // kernel indexers
    int ki;
    int kj;

    int inputIdx;

    float w;
    float inputVal;
        
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
    return conv;
}

template float Convolution(Buffer* inputBuffer,
                     std::vector<std::vector<float>>& kernel,
                     int& kernelHeight, int& kernelWidth,
                     int& startLine, int& startCol, int c, 
                     int& channels, int& cols);

template float Convolution(Buffer* inputBuffer,
                           std::vector<std::vector<byte>>& kernel,
                           int& kernelHeight, int& kernelWidth,
                           int& startLine, int& startCol, int c, 
                           int& channels, int& cols);


bool Hysterisis(Buffer* inputBuffer, 
                int& i, int& j,
                int& rows, int& cols)
{
    int iMem = i % inputBuffer->lines;

    // already strong
    if (inputBuffer->memory[iMem*cols+j] == 255) {return true;}
    
    // already non relevant 
    if (inputBuffer->memory[iMem*cols+j] == 0) {return false;}

    
    // up
    if ((i-1 >= 0 &&
         inputBuffer->memory[((i-1) % inputBuffer->lines)*cols+j] == 255))
        {return true;}

    // down
    if ((i+1 < rows && 
         inputBuffer->memory[((i+1) % inputBuffer->lines)*cols+j] == 255))
        {return true;}
    // left
    if ((j-1 >= 0 && 
         inputBuffer->memory[iMem*cols+(j-1)] == 255))
        {return true;}
    // right
    if ((j+1 >= 0 && 
         inputBuffer->memory[iMem*cols+(j+1)] == 255))
        {return true;}

    // DIAGONALS

    // left up
    if ((i-1 >= 0 && j-1 >= 0 && 
         inputBuffer->memory[((i-1) % inputBuffer->lines)*cols+(j-1)] == 255))
        {return true;}
    // right up
    if ((i-1 >= 0 && j+1 < cols && 
         inputBuffer->memory[((i-1) % inputBuffer->lines)*cols+(j+1)] == 255))
        {return true;}
    // left down
    if ((i+1 < rows && j-1 >= 0 && 
         inputBuffer->memory[((i+1) % inputBuffer->lines)*cols+(j-1)] == 255))
        {return true;}
    // right down
    if ((i+1 < rows && j+1 < cols && 
         inputBuffer->memory[((i+1) % inputBuffer->lines)*cols+(j+1)] == 255))
        {return true;}

    return false;
}
#include "CVfunctions.h"


template<typename KernelType>
float MatMul(Buffer* inputBuffer,
             std::vector<std::vector<KernelType>>& kernel,
             int& kernelHeight, int& kernelWidth,
             int& ai, int& aj, int& c) 
{
    float sumProduct = 0;

    // this works only for odd kernel sizes currently
    int starti = ai-kernelHeight/2;
    int startj = aj-kernelWidth/2;

    int endi = ai+kernelHeight/2;
    int endj = aj+kernelWidth/2;

    for (int i = starti; i < endi+1; i++) {
        
        if (i < 0 || i >= inputBuffer->rows) {
            continue;
        }
        for (int j = startj; j < endj+1; j++) {
        
            if (j < 0 || j >= inputBuffer->cols) {
                continue;
            }
            int ki = i-starti;
            int kj = j-startj;

            int lineMemoryIdx = inputBuffer->LineMemoryIndex(i);
            int inputIdx = (lineMemoryIdx + 
                            j*inputBuffer->channels + 
                            c);
            float inputVal = ((float) inputBuffer->memory[inputIdx]);
            float w = kernel[ki][kj];

            sumProduct += inputVal*w;
        }
    }
    return sumProduct;
}

template float MatMul(Buffer* inputBuffer,
                      std::vector<std::vector<float>>& kernel,
                      int& kernelHeight, int& kernelWidth,
                      int& ai, int& aj, int& c);

bool Hysterisis(Buffer* inputBuffer,
                int& ai, int& aj, int& c)
{
    // check if anchor is strong or non relevant

    int anchorlineMemoryIdx = inputBuffer->LineMemoryIndex(ai);
    int anchorIdx = (anchorlineMemoryIdx + 
                    aj*inputBuffer->channels + 
                    c);

    // already strong
    if (inputBuffer->memory[anchorIdx] == 255) {return true;}
    
    // already non relevant 
    if (inputBuffer->memory[anchorIdx] == 0) {return false;}

    // check if pixels in neighbourhood are strong

    int starti = ai-1;
    int startj = aj-1;

    int endi = ai+1;
    int endj = aj+1;

    for (int i = starti; i < endi+1; i++) {

        int lineMemoryIdx = inputBuffer->LineMemoryIndex(i);
        
        if (i < 0 || i >= inputBuffer->rows) {
            continue;
        }
        for (int j = startj; j < endj+1; j++) {
        
            if (j < 0 || j >= inputBuffer->cols) {
                continue;
            }
            int inputIdx = (lineMemoryIdx + 
                            j*inputBuffer->channels + 
                            c);
            
            if (inputIdx == anchorIdx) {continue;}

            if (inputBuffer->memory[inputIdx] == 255) {
                // inputBuffer->memory[anchorIdx] = 255;
                return true;
            }
        }
    }

    return false;
}

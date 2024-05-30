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
        
        for (int j = startj; j < endj+1; j++) {
            
            int ki = i-starti;
            int kj = j-startj;
        
            int iReflect = ReflectIndex(i, inputBuffer->rows);
            int jReflect = ReflectIndex(j, inputBuffer->cols);

            int lineMemoryIdx = inputBuffer->LineMemoryIndex(iReflect);
            int inputIdx = (lineMemoryIdx + 
                            jReflect*inputBuffer->channels + 
                            c);
            float inputVal = ((float) inputBuffer->Memory<byte>()[inputIdx]);
            float w = kernel[ki][kj];

            sumProduct += inputVal*w;
        }
    }
    return sumProduct;
}

// Function to handle border reflection for index i
int ReflectIndex(int i, int maxIdx) {
    int iNew = i;
    if (i < 0) {iNew = -i - 1;} 
    else if (i >= maxIdx) {iNew = 2 * maxIdx - 1 - i;}
    return iNew;
}

template float MatMul(Buffer* inputBuffer,
                      std::vector<std::vector<float>>& kernel,
                      int& kernelHeight, int& kernelWidth,
                      int& ai, int& aj, int& c);

bool Hysterisis(Buffer* inputBuffer,
                int& ai, int& aj, int& c,
                uint16_t& lowThreshold, uint16_t& highThreshold)
{
    // check if anchor is strong or non relevant

    int anchorlineMemoryIdx = inputBuffer->LineMemoryIndex(ai);
    int anchorIdx = (anchorlineMemoryIdx + 
                    aj*inputBuffer->channels + 
                    c);

    // already strong
    if (inputBuffer->Memory<uint16_t>()[anchorIdx] > highThreshold) {return true;}
    
    // already non relevant 
    if (inputBuffer->Memory<uint16_t>()[anchorIdx] < lowThreshold) {return false;}

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

            if (inputBuffer->Memory<uint16_t>()[inputIdx] > highThreshold) {
                inputBuffer->Memory<uint16_t>()[anchorIdx] = 255;
                return true;
            }
        }
    }
    inputBuffer->Memory<uint16_t>()[anchorIdx] = 0;
    return false;
}

#include "CVfunctions.h"

void Convolution(byte* inputBuffer, byte* outputBuffer, 
                 std::vector<std::vector<float>>& kernel, 
                 int& kernelHeight, int& kernelWidth,
                 int& si, int& sj, int& inputStartLine, int& outputLine,
                 int& channels, int& cols, int& rows, 
                 int& padHeight, int& padWidth,
                 int& strideHeight, int& strideWidth) {
    
    // indexer for line in cicular input buffer
    int inputLine = inputStartLine;

    // padded image indexers
    int ii;
    int jj;
    bool nonPaddedArea;

    // where in output
    int outi = (si + padHeight)/strideHeight;
    int outj = (sj + padWidth)/strideWidth;

    int outidx = (outi+outputLine)*cols+outj*channels;

    // si and sj are the start indices (can be negative if in padded area)
    for (int c = 0; c < channels; c++) {
        int conv = 0;
        for (int i = 0; i < kernelHeight; i++) {

            // indexer for line in cicular input buffer
            inputLine = (inputLine+i)%kernelHeight;

            for (int j = 0; j < kernelWidth; j++) {
                
                // padded image indexers
                ii = i + si;
                jj = j + sj; 

                // non padded area
                nonPaddedArea = ii >= 0 && jj >= 0 && ii < rows && jj < cols;

                if (nonPaddedArea) {
                    int w = kernel[i][j];
                    conv+=w*((float)inputBuffer[inputLine*cols+jj*channels+c]);
                }
            }
        }
        outputBuffer[outidx+c] = (byte) (conv);
    }
}
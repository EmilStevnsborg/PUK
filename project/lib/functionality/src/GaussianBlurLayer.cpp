#include "GaussianBlurLayer.h"
#include <cstdio>

GaussianBlurLayer::GaussianBlurLayer(int inputChannels, 
                                     int inputRows, 
                                     int inputCols) 
    
    : Layer(inputChannels, inputRows, inputCols), 
        inputBuffer(inputChannels, inputRows, inputCols, 5)
{
    this->inputChannels = inputChannels;
    this->inputRows = inputRows;
    this->inputCols = inputCols;

    this->kernelHeight = 5;
    this->kernelWidth = 5;
    this->strideHeight = 1;
    this->strideWidth = 1;
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;
    this->kernel = {
        {0.003, 0.013, 0.022, 0.013, 0.003},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.022, 0.097, 0.159, 0.097, 0.022},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.003, 0.013, 0.022, 0.013, 0.003},   
    };
}

// stream a blurred line to output: necssarry lines already in buffer
void GaussianBlurLayer::Stream(Buffer* outputBuffer, int line) {

    // We are asked for line. 
    // Computation requires [line-padHeight,..., line+padHeight] from input
    int startLine = line-padHeight;

    // Where the line should be placed in the memory of the outputBuffer
    int outMemIdx = (outputBuffer->lineInserts % outputBuffer->lines) * 
                     outputBuffer->bytesLine;


    // iterate over pixels in output line
    for (int startCol = -padWidth; startCol < inputCols; startCol++) {
        // printf("startLine %d startCol %d\n", startLine, startCol);
        Convolution(&inputBuffer, outputBuffer,
                    outMemIdx, kernel, kernelHeight, kernelWidth,
                    startLine, startCol,
                    inputChannels, inputCols);
        
        outMemIdx += outputBuffer->channels;
    }
}
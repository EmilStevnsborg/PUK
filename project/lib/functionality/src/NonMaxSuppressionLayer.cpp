#include "NonMaxSuppressionLayer.h"

NonMaxSuppressionLayer::NonMaxSuppressionLayer(int inputChannels, 
                                               int inputRows, 
                                               int inputCols)     
    : Layer(inputChannels, inputRows, inputCols), // Initialize base class
      inputBuffer(inputChannels, inputRows, inputCols, 3) {
}

void NonMaxSuppressionLayer::Stream(Buffer* outputBuffer, int line) {
    
}
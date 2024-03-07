#include "NonMaxSuppressionLayer.h"

NonMaxSuppressionLayer::NonMaxSuppressionLayer(int inputChannels, 
                                               int inputRows, 
                                               int inputCols,
                                               int kernelHeight, 
                                               int kernelWidth,
											   float lowThreshold,
											   float highThreshold)     
    : Layer(inputChannels, inputRows, inputCols),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight, true, 1) 
{
    this->kernelHeight = kernelHeight;
    this->kernelWidth = kernelWidth;

    // integer division is floored
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;

	this->lowThreshold = lowThreshold;
	this->highThreshold = highThreshold;
}

// inputBuffer has extraMemory as well, where the gradient angle is
void NonMaxSuppressionLayer::Stream(Buffer* outputBuffer, int line) {

    // the line index in the memory of the outputBuffer
    int outLineMemIdx = outputBuffer->LineMemoryIndex(line);
    
    // anchor line
    int inLineMemIdx = inputBuffer.LineMemoryIndex(line);

	int i = line;

	// for each pixel compute non-max uppression

    for (int j = 0; j < outputBuffer->cols; j++) {
        for (int c = 0; c < outputBuffer->channels; c++) {

			int inputIdx = inLineMemIdx + j*inputBuffer.channels + c;

			byte inputVal = inputBuffer.memory[inputIdx];
			byte angle = inputBuffer.extraMemory[inputIdx];

			byte val = inputVal;

			// up down
			if (angle == 0) {

				int qi = i-1;
				int qj = j;
				
				int ri = i+1;
				int rj = j;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[qIdx] > inputVal) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[rIdx] > inputVal) {val = 0;}
				}
			}
			// diagonal up 
			else if (angle == 1) {

				int qi = i+1;
				int qj = j-1;
				
				int ri = i-1;
				int rj = j+1;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[qIdx] > inputVal) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[rIdx] > inputVal) {val = 0;}
				}
			}
			// horizontal
			else if (angle == 2) {

				int qi = i;
				int qj = j-1;
				
				int ri = i;
				int rj = j+1;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[qIdx] > inputVal) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[rIdx] > inputVal) {val = 0;}
				}
			}
			// diagonal down
			else {

				int qi = i-1;
				int qj = j-1;
				
				int ri = i+1;
				int rj = j+1;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[qIdx] > inputVal) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.memory[rIdx] > inputVal) {val = 0;}
				}
			}
			if (val >= (byte) highThreshold) {
				val = 255;
			} else if (val >= (byte) lowThreshold) {
				val = 127;
			}
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;
			outputBuffer->memory[outIdx] = val;
		}
    }
}
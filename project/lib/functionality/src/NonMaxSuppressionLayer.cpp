#include "NonMaxSuppressionLayer.h"

NonMaxSuppressionLayer::NonMaxSuppressionLayer(int inputChannels, 
                                               int inputRows, 
                                               int inputCols,
                                               int kernelHeight, 
                                               int kernelWidth,
											   byte lowThreshold,
											   byte highThreshold)     
    : Layer(inputChannels, inputRows, inputCols),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight, true) 
{
	// this is 1 always
    this->inputChannels = inputChannels;

    this->inputRows = inputRows;
    this->inputCols = inputCols;

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

    // Where the line should be placed in the memory of the outputBuffer
    int outMemIdx = (line % outputBuffer->lines) * 
                     outputBuffer->bytesLine;
    
    // the line is located at
    int memoryLine = line % inputBuffer.lines;

	// for each pixel compute non-max uppression
    for (int j = 0; j < inputCols; j++) {
		for (int c = 0; c < inputBuffer.channels; c++) {
			int inputIdx = memoryLine*inputBuffer.bytesLine + j*inputBuffer.channels + c;

			byte inputVal = inputBuffer.memory[inputIdx];
			byte angle = inputBuffer.extraMemory[inputIdx];

			byte val = inputVal;

			// up down
			if (angle == 0) {
				int half = padHeight;
				// gradient direction values
				for (int otherL = line-half; otherL < line+1+half; otherL++) {

					// the line in memory
					int otherMemoryLine = otherL % inputBuffer.lines;

					// the index in memory
					int otherIdx = (otherMemoryLine*inputBuffer.bytesLine+
									j);

					if (otherL >= 0 && otherL < inputRows && otherIdx != inputIdx) {
						byte otherVal = inputBuffer.memory[otherIdx];
						if (otherVal > inputVal) {val = 0; break;}
					}
				}
			}
			// diagonal up 
			else if (angle == 1) {

				int half = padWidth;
				// gradient direction values
				for (int k = -half; k < 1+half; k++) {
					
					int otherL = line+k;
					int otherJ = j+k;
					// the line in memory
					int otherMemoryLine = otherL % inputBuffer.lines;

					// the index in memory
					int otherIdx = (otherMemoryLine*inputBuffer.bytesLine+
									otherJ);

					if (((otherL >= 0 && otherL < inputRows) &&
							(otherJ >= 0 && otherJ < inputCols) && 
							(otherIdx != inputIdx))) 
					{
						byte otherVal = inputBuffer.memory[otherIdx];
						if (otherVal > inputVal) {val = 0; break;}
					}
				}
			}
			// horizontal
			else if (angle == 2) {

				int half = kernelWidth/2;
				// gradient direction values
				for (int otherJ = line-half; otherJ < line+1+half; otherJ++) {

					// the line in memory
					int otherMemoryLine = memoryLine;

					// the index in memory
					int otherIdx = (otherMemoryLine*inputBuffer.bytesLine+
									otherJ);

					if (otherJ >= 0 && otherJ < inputCols && otherIdx != inputIdx) {
						byte otherVal = inputBuffer.memory[otherIdx];
						if (otherVal > inputVal) {val = 0; break;}
					}
				}
			}
			// diagonal down
			else {

				int half = kernelHeight/2;
				// gradient direction values
				for (int k = -half; k < 1+half; k++) {
					
					int otherL = line-k;
					int otherJ = j+k;
					// the line in memory
					int otherMemoryLine = otherL % inputBuffer.lines;

					// the index in memory
					int otherIdx = (otherMemoryLine*inputBuffer.bytesLine+
									otherJ);

					if (((otherL >= 0 && otherL < inputRows) &&
							(otherJ >= 0 && otherJ < inputCols) && 
							(otherIdx != inputIdx))) 
					{
						byte otherVal = inputBuffer.memory[otherIdx];
						if (otherVal > inputVal) {val = 0; break;}
					}
				}
			}
			if (val >= highThreshold) {
				val = 255;
			} else if (val >= lowThreshold) {
				val = 127;
			}
			outputBuffer->memory[outMemIdx+j*inputBuffer.channels+c] = val;
		}
		outMemIdx += outputBuffer->channels;
    }
}
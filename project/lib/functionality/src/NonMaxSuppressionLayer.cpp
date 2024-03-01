#include "NonMaxSuppressionLayer.h"

NonMaxSuppressionLayer::NonMaxSuppressionLayer(int inputChannels, 
                                               int inputRows, 
                                               int inputCols,
                                               int kernelHeight, 
                                               int kernelWidth)     
    : Layer(inputChannels, inputRows, inputCols),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight, true) 
{
    this->inputChannels = inputChannels;
    this->inputRows = inputRows;
    this->inputCols = inputCols;

    this->kernelHeight = kernelHeight;
    this->kernelWidth = kernelWidth;

    // integer division is floored
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;
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
        for (int c = 0; c < outputBuffer->channels; c++) {
			int inputIdx = memoryLine*inputBuffer.bytesLine + j*inputChannels + c;

			byte inputVal = inputBuffer.memory[inputIdx];
			byte angle = inputBuffer.extraMemory[inputIdx];

			byte max = inputVal;

			// up down
			if (angle == 0) {
				int half = padHeight;
				// gradient direction values
				for (int otherL = line-half; otherL < line+1+half; otherL++) {

					// the line in memory
					int otherMemoryLine = otherL % inputBuffer.lines;

					// the index in memory
					int otherIdx = (otherMemoryLine*inputBuffer.bytesLine+
									j*inputChannels+c);

					if (otherL >= 0 && otherL < inputRows && otherIdx != inputIdx) {
						byte otherVal = inputBuffer.memory[otherIdx];
						max = std::max(max, otherVal);
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
									otherJ*inputChannels+c);

					if (((otherL >= 0 && otherL < inputRows) &&
					     (otherJ >= 0 && otherJ < inputCols) && 
						 (otherIdx != inputIdx))) 
					{
						byte otherVal = inputBuffer.memory[otherIdx];
						max = std::max(max, otherVal);
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
									otherJ*inputChannels+c);

					if (otherJ >= 0 && otherJ < inputCols && otherIdx != inputIdx) {
						byte otherVal = inputBuffer.memory[otherIdx];
						max = std::max(max, otherVal);
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
									otherJ*inputChannels+c);

					if (((otherL >= 0 && otherL < inputRows) &&
					     (otherJ >= 0 && otherJ < inputCols) && 
						 (otherIdx != inputIdx))) 
					{
						byte otherVal = inputBuffer.memory[otherIdx];
						max = std::max(max, otherVal);
					}
				}
			}
            outputBuffer->memory[outMemIdx+c] = max;
        }
        outMemIdx += outputBuffer->channels;
    }
}
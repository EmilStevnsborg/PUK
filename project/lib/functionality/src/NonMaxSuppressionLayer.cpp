#include "NonMaxSuppressionLayer.h"

NonMaxSuppressionLayer::NonMaxSuppressionLayer(int inputChannels, 
                                               int inputRows, 
                                               int inputCols)     
    : Layer(),
      // buffer stores 16 bit gradient magnitudes and angles in extra memory
      inputBuffer(inputChannels, inputRows, inputCols, 3, true, false) 
{
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
            int outIdx = outLineMemIdx+j*(outputBuffer->channels)+c;

			uint16_t gradientMagnitude = inputBuffer.Memory<uint16_t>()[inputIdx];
			byte angle = inputBuffer.extraMemory[inputIdx];

			uint16_t val = gradientMagnitude;

			// up down angle (horizontal edge)
			if (angle == 0) {

				int qi = i;
				int qj = j-1;
				
				int ri = i;
				int rj = j+1;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[qIdx] > gradientMagnitude) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[rIdx] > gradientMagnitude) {val = 0;}
				}
			}
			// diagonal up angle (diagonal down edge)
			else if (angle == 45) {
				int qi = i+1;
				int qj = j-1;
				
				int ri = i-1;
				int rj = j+1;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[qIdx] > gradientMagnitude) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[rIdx] > gradientMagnitude) {val = 0;}
				}
			}
			// horizontal angle (up down edge)
			else if (angle == 90) {

				int qi = i-1;
				int qj = j;
				
				int ri = i+1;
				int rj = j;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[qIdx] > gradientMagnitude) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[rIdx] > gradientMagnitude) {val = 0;}
				}
			}
			// diagonal down angle (diagonal up edge)
			else {

				int qi = i-1;
				int qj = j-1;
				
				int ri = i+1;
				int rj = j+1;

				if (qi >= 0 && qj >= 0) {
					int qIdx = (inputBuffer.LineMemoryIndex(qi) + 
								qj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[qIdx] > gradientMagnitude) {val = 0;}
				}
				if (ri < inputBuffer.rows && rj < inputBuffer.cols) {
					int rIdx = (inputBuffer.LineMemoryIndex(ri) + 
								rj*inputBuffer.channels + 
								c);
					if (inputBuffer.Memory<uint16_t>()[rIdx] > gradientMagnitude) {val = 0;}
				}
			}
			outputBuffer->Memory<uint16_t>()[outIdx] = (uint16_t) val;
		}
    }
}

// streaming line this layer, what lines does inputbuffer need to do that
std::vector<int> NonMaxSuppressionLayer::NextLines(int streamingLine) {
    std::vector<int> nextLines;

    // if buffer is already full, then nothing
    if (inputBuffer.lineInserts == inputBuffer.rows) {
        return nextLines;
    }
    
    int startLine = inputBuffer.lineInserts;
    int endLine = streamingLine + 1;
    
    for (int nextLine = startLine; nextLine <= endLine; nextLine++) {
        if (nextLine < inputBuffer.rows) {
            nextLines.push_back(nextLine);
        }
    }
    return nextLines;
}
#include "Host.h"
#include <cstdio>

Host::Host(Camera* cam, int channels, int rows, int cols)
{
    this->camSensor = cam;
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;
    // sensor we stream data from
    this->camSensor = cam;
}

void Host::CannyEdge(Buffer* outputBuffer) {}

// Gaussian filter which blurs camera input and outputs an image of same size
// The kernel is 5x5
void Host::GaussianBlur(Buffer* outputBuffer) {
    // init inputs for computation
    GaussianBlurLayer gaussianBlurLayer(channels, rows, cols);

    // stream intital lines from cam into first input
    for (int camLine = 0; camLine < gaussianBlurLayer.inputBuffer.lines; camLine++) {
        camSensor->Stream(&gaussianBlurLayer.inputBuffer, 
                          camLine);
        gaussianBlurLayer.inputBuffer.LineInserted();
    }

    printf("bytes allocated %d\n", gaussianBlurLayer.inputBuffer.bytesAllocated);

    // compute each line in the outputBuffer
    for (int line = 0; line < outputBuffer->lines; line++) {
        // printf("line %d\n", line);
        gaussianBlurLayer.Stream(outputBuffer, line);

        // for each new line (if `line - padHeight >= 0`) stream from cam
        if (line - gaussianBlurLayer.padHeight >= 0) {
            
            // the next camline
            if (line < rows) {
                camSensor->Stream(&gaussianBlurLayer.inputBuffer, 
                                  line);

                gaussianBlurLayer.inputBuffer.LineInserted();
            }
        }
        outputBuffer->LineInserted();
    }

    gaussianBlurLayer.inputBuffer.FreeMemory();
}
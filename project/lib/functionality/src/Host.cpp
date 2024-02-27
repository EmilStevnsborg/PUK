#include "Host.h"

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
void Host::GaussianBlur(Buffer* outputBuffer, 
                        int kernelHeight, int kernelWidth, 
                        double sigmaX, double sigmaY) {
    // init inputs for computation
    GaussianBlurLayer gaussianBlurLayer(channels, rows, cols, 
                                        kernelHeight, kernelWidth, 
                                        sigmaX, sigmaY);

    // stream intital lines from cam into first input
    for (int camLine = 0; camLine < gaussianBlurLayer.inputBuffer.lines; camLine++) {
        camSensor->Stream(&gaussianBlurLayer.inputBuffer, 
                          camLine);
        gaussianBlurLayer.inputBuffer.LineInserted();
    }

    // compute each line in the outputBuffer
    for (int line = 0; line < outputBuffer->lines; line++) {
        // printf("line %d\n", line);
        gaussianBlurLayer.Stream(outputBuffer, line);
        
        int camLine = line + gaussianBlurLayer.padHeight + 1;

        // stream from cam if convolution is between top and bottom padding
        if (line - gaussianBlurLayer.padHeight >= 0 && camLine < rows) {
            camSensor->Stream(&gaussianBlurLayer.inputBuffer, 
                                camLine);

            gaussianBlurLayer.inputBuffer.LineInserted();
        }
        outputBuffer->LineInserted();
    }

    gaussianBlurLayer.inputBuffer.FreeMemory();
}

void Host::NonMaxSuppression(Buffer* outputBuffer) {
    NonMaxSuppressionLayer nonMaxSuppressionLayer(channels, rows, cols);
}
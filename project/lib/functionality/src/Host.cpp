#include "Host.h"

Host::Host(Camera* cam, int channels, int rows, int cols) {
    this->camSensor = cam;
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;
};

void Host::CannyEdge(int kernelHeight, int kernelWidth) {

}

// Gaussian filter which blurs camera input and outputs an image of same size
// The kernel is 5x5
void Host::GaussianFilter() {
    int kernelHeight = 5;
    int kernelWidth = 5;
    int strideHeight = 1;
    int strideWidth = 1;
    int padHeight = kernelHeight/2;
    int padWidth = kernelWidth/2;
    std::vector<std::vector<float>> kernel {
        {0.003, 0.013, 0.022, 0.013, 0.003},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.022, 0.097, 0.159, 0.097, 0.022},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.003, 0.013, 0.022, 0.013, 0.003},   
    };

    // allocate only memory for kernel size
    int bytesLine = cols*channels;
    int lines = 5;

    // circular buffer
    Buffer inputBuffer = Buffer(lines, bytesLine);

    int inputStartLine = 0;

    // stream inital 3 lines for 5x5 kernel (2 pad height)
    for (int line = 0; line < 3; line++) {
        int startIdx = line*bytesLine;
        camSensor->Stream(inputBuffer.memory, line, startIdx);
    }
    
    // buffers pixels for entire image
    Buffer outputBuffer = Buffer(rows, bytesLine);
    
    int si = (-1)*padHeight;
    int sj = (-1)*padWidth;

    int outputLine = 0;
    int outputCol = 0;

    // streaming from camera sensor and computing output buffer
    while (outputLine < rows) {
        
        // one convolution for each ouput pixel
        Convolution(inputBuffer.memory, outputBuffer.memory, 
                    kernel, 
                    kernelHeight, kernelWidth,
                    si, sj, inputStartLine, outputLine,
                    channels, cols, rows, 
                    padHeight, padWidth, strideHeight, strideWidth);
        
        // next col (pixel in line)
        outputCol += 1;

        if (outputCol == bytesLine) {

            // next line in output
            outputLine += 1;
            outputCol = 0;

            // kernel shifts with strideHeight
            inputStartLine += 1;
            // stream next line into free line in inputBuffer
            
        }
    }

    // free memory
    inputBuffer.FreeMemory();
    outputBuffer.FreeMemory();
}
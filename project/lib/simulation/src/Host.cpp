#include "Host.h"
#include <vector>

Host::Host(CamSimulator* camSimulator, int channels, int rows, int cols) {
    this->camSensor = camSimulator;
    this->channels = channels;
    this->rows = rows;
    this->cols = cols;

    if (camSimulator.Rows() != this->rows ||
        camSimulator.Cols() != this->cols ||
        camSimulator.channels() != this->channels) {

            printf("Camera and host specifics\n"
                   "camSimulator.Rows() %d == this->rows %d\n"
                   "camSimulator.Cols() %d == this->cols %d\n"
                   "camSimulator.Channels)= %d == this->channels %d\n", 
                    camSimulator.Rows(), this->rows, camSimulator.Cols(),
                    this->cols, camSimulator.Channels(), this->channels);
            
            throw std::runtime_error("Camera properties not equal to"
                                     "while storing");
    }
}

void Host::CannyEdge(int kernelHeight, int kernelWidth) {

}

// Gaussian filter which blurs camera input and outputs an image of same size
// The kernel is 5x5
void Host::GaussianFilter() {
    int kernelHeight = 5;
    int kernelWidth = 5;
    int padHeight = kernelHeight/2;
    int padWidth = kernelWidth/2;
    vector<vector<float>> kernel {
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
    inputBuffer = Buffer(lines, bytesLine);

    int inputStartLine = 0;

    // stream inital 3 lines for 5x5 kernel (2 pad height)
    for (int line = 0; line < 3; line++) {
        int startIdx = line*bytesLine;
        camSensor.Stream(inputBuffer.memory, line, startIdx);
    }
    
    // buffers pixels for entire image
    outputBuffer = Buffer(rows, bytesLine);
    
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
                    padHeight, padWidth);
        
        // next col (pixel in line)
        outputCol += 1;

        if (outputCol == bytesLine) {

            // next line in output
            outputLine += 1;
            outputCol = 0;

            // kernel shifts with strideHeight
            inputStartLine += 1;
        }
    }

    // free memory
    inputBuffer.FreeMemory();
    outputBuffer.FreeMemory();
}
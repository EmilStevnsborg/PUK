#include "GaussianBlurLayer.h"

GaussianBlurLayer::GaussianBlurLayer(int inputChannels, int inputRows, 
                                     int inputCols,
                                     int kernelHeight, int kernelWidth, 
                                     double sigmaX, double sigmaY)
    : Layer(inputChannels, inputRows, inputCols),
      inputBuffer(inputChannels, inputRows, inputCols, kernelHeight)
{
    this->inputChannels = inputChannels;
    this->inputRows = inputRows;
    this->inputCols = inputCols;

    this->kernelHeight = kernelHeight;
    this->kernelWidth = kernelWidth;
    
    this->sigmaX = sigmaX;
    this->sigmaY = sigmaY;

    // integer division is floored
    this->padHeight = kernelHeight/2;
    this->padWidth = kernelWidth/2;
    
    // kernel
    this->GetKernel();
}

// stream a blurred line to output: necssarry lines already in buffer
void GaussianBlurLayer::Stream(Buffer* outputBuffer, int line) {

    // Computation requires [line-padHeight,..., line+padHeight] from input
    int startLine = line-padHeight;

    // Where the line should be placed in the memory of the outputBuffer
    int outMemIdx = (outputBuffer->lineInserts % outputBuffer->lines) * 
                     outputBuffer->bytesLine;


    for (int startCol = -padWidth; startCol < inputCols; startCol++) {
        for (int c = 0; c < outputBuffer->channels; c++) {
            
            float conv = Convolution<float>(&inputBuffer, kernel, 
                                            kernelHeight, kernelWidth,
                                            startLine, startCol, c,
                                            inputChannels, inputCols);
            
            outputBuffer->memory[outMemIdx+c] = (byte) (std::floor(conv));
        }
        outMemIdx += outputBuffer->channels;
    }
}

void GaussianBlurLayer::GetKernel() {
    // Initialize the kernel with appropriate size
    this->kernel = std::vector<std::vector<float>>(kernelHeight,
                                        std::vector<float>(kernelWidth));
    
    const float epsilon = 1e-5;

    float sum_ = 0;

    // If sigmaY is zero it is set equal to sigmaX
    if (sigmaY < epsilon) {
        sigmaY = sigmaX;
    }

    // If sigmaY and sigmaX are both zero, compute them from kernelHeight and kernelWidth
    if (sigmaY < epsilon && sigmaX < epsilon) {
        sigmaX = 0.3 * ((kernelWidth - 1) * 0.5 - 1) + 0.8;
        sigmaY = 0.3 * ((kernelHeight - 1) * 0.5 - 1) + 0.8;
    }
    // If sigmaY is zero, set it equal to sigmaX
    else if (sigmaY < epsilon) {
        sigmaY = sigmaX;
    }
    
    // Calculate the center of the kernel
    int centerX = kernelWidth / 2;
    int centerY = kernelHeight / 2;
    

    // Fill the kernel with Gaussian values
    for (int i = 0; i < kernelHeight; i++) {
        for (int j = 0; j < kernelWidth; j++) {
            // Compute distance from center
            int x = j - centerX;
            int y = i - centerY;
            // Calculate Gaussian value
            float value = (exp((-(x * x / (2 * sigmaX * sigmaX) + 
                                  y * y / (2 * sigmaY * sigmaY)))));
            sum_ += value;
            kernel[i][j] = value;
        }
    }

    // normalize
    for (int i = 0; i < kernelHeight; i++) {
        for (int j = 0; j < kernelWidth; j++) {
            kernel[i][j] /= sum_;
        }
    }
}
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

void Host::PopulateBuffers(std::vector<std::unique_ptr<Layer>>& layers, 
                           Buffer* outputBuffer, 
                           int currentLayerIdx, int line)
{
    if (outputBuffer != nullptr) {
        layers[currentLayerIdx]->Stream(outputBuffer, line);
    }
    
    // next line that needs to be put into layer's input buffer
    int nextLine = line + layers[currentLayerIdx]->PadHeight()+1;
    
    // populate inputBuffer
    if ((line - layers[currentLayerIdx]->PadHeight() >= 0) &&
        (nextLine < layers[currentLayerIdx]->InputBuffer()->rows))
    {
        if (currentLayerIdx == (int) layers.size()-1) {
            camSensor->Stream(layers[currentLayerIdx]->InputBuffer(), 
                              nextLine);
        } else {
            PopulateBuffers(layers, layers[currentLayerIdx]->InputBuffer(),
                            currentLayerIdx+1, nextLine);
        }
        layers[currentLayerIdx]->InputBuffer()->LineInserted();
    }
}

// This function streams an entire output to outputBuffer
void Host::StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers, 
                             Buffer* outputBuffer) 
{
    // populate inputBuffer of first layer with cam data
    for (int camLine = 0; 
         camLine < layers[layers.size()-1]->InputBuffer()->lines; 
         camLine++) 
    {
        camSensor->Stream(layers[layers.size()-1]->InputBuffer(), 
                          camLine);
        layers[layers.size()-1]->InputBuffer()->LineInserted();
    }

    // populate remainding layers' input buffers recursively
    for (int layerIdx = layers.size()-2; layerIdx >= 0; layerIdx--) {
        for(int line = 0; line < layers[layerIdx]->InputBuffer()->lines; line++) {
            PopulateBuffers(layers, 
                            layers[layerIdx]->InputBuffer(), 
                            layerIdx+1, line);

            layers[layerIdx]->InputBuffer()->LineInserted();
        }
    }

    // Compute each line in the outputBuffer
    for (int line = 0; line < outputBuffer->lines; line++) {

        // Update input buffers of layers recursively for the next line
        PopulateBuffers(layers, outputBuffer, 0, line);
        
        // Indicate that one line has been inserted into the output buffer
        outputBuffer->LineInserted();
    }

    // Free memory for input buffers of each layer
    for (auto& layer : layers) {
        layer->InputBuffer()->FreeMemory();
    }
}



// Gaussian filter which blurs camera input and outputs an image of same size
void Host::GaussianBlur(Buffer* outputBuffer, 
                        int kernelHeight, int kernelWidth, 
                        double sigmaX, double sigmaY) 
{   
    std::vector<std::unique_ptr<Layer>> layers;

    auto gaussianBlurLayer = std::make_unique<GaussianBlurLayer>(channels, rows, cols, 
                                                                 kernelHeight, kernelWidth, 
                                                                 sigmaX, sigmaY);

    layers.push_back(std::move(gaussianBlurLayer));

    StreamingPipeLine(layers, outputBuffer);
}

void Host::Sobel(Buffer* outputBuffer) {
    std::vector<std::unique_ptr<Layer>> layers;

    auto grayScaleLayer = std::make_unique<GrayScaleLayer>(channels, rows, cols);

    auto gaussianBlurLayer = std::make_unique<GaussianBlurLayer>(1, 
                                    rows, cols, 3, 3, 0, 0);

    auto sobelLayer = std::make_unique<SobelLayer>(1, rows, cols, 3, 3);

    layers.push_back(std::move(sobelLayer));
    layers.push_back(std::move(grayScaleLayer));
    
    StreamingPipeLine(layers, outputBuffer);
}


void Host::CannyEdge(Buffer* outputBuffer, float lowThreshold, float highThreshold) 
{

    std::vector<std::unique_ptr<Layer>> layers;

    // init layers for computation

    auto grayScaleLayer = std::make_unique<GrayScaleLayer>(channels, rows, cols);

    auto gaussianBlurLayer = std::make_unique<GaussianBlurLayer>(1, 
                                    rows, cols, 5, 5, 0, 0);

    auto sobelLayer = std::make_unique<SobelLayer>(1, rows, cols, 3, 3);

    auto nmxLayer = std::make_unique<NonMaxSuppressionLayer>(1, rows, cols, 
                                                             3, 3, lowThreshold,
                                                             highThreshold);
    auto hysterisisLayer = std::make_unique<HysterisisLayer>(1, rows, cols, 3, 3);

    layers.push_back(std::move(hysterisisLayer));
    layers.push_back(std::move(nmxLayer));
    layers.push_back(std::move(sobelLayer));
    layers.push_back(std::move(gaussianBlurLayer));
    layers.push_back(std::move(grayScaleLayer));

    // The first required lines for nmx's stream function exists in its input buffer

    StreamingPipeLine(layers, outputBuffer);
}
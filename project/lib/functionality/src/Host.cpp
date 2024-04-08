#include "Host.h"

// undefined Host
Host::Host() {}

void Host::Configure(Camera* cam)
{
    this->camSensor = cam;
    this->channels = cam->Channels();
    this->rows = cam->Rows();
    this->cols = cam->Cols();
}

void Host::PopulateBuffers(std::vector<std::unique_ptr<Layer>>& layers,
                           Buffer* outputBuffer,
                           int currentLayerIdx, int line)
{    
    // in order to stream line, inputBuffer needs the next lines:
    std::vector<int> nextLines = layers[currentLayerIdx]->NextLines(line);
    
    // populate inputBuffer with each line needed (some might already be there)
    for (auto& nextLine: nextLines)
    {
        if (currentLayerIdx == 0) {
            if (camSensor->snapshot) {
                camSensor->Stream(layers[currentLayerIdx]->InputBuffer());
                layers[currentLayerIdx]->InputBuffer()->lineInserts = rows;
                break;
            } 
            // line scan
            else {
                camSensor->Stream(layers[currentLayerIdx]->InputBuffer(), 
                                  nextLine);
            }
        } else {
            PopulateBuffers(layers, layers[currentLayerIdx]->InputBuffer(),
                            currentLayerIdx-1, nextLine);
        }
        // line has been inserted
        layers[currentLayerIdx]->InputBuffer()->LineInserted();
    }
    // stream the layer to the output
    layers[currentLayerIdx]->Stream(outputBuffer, line);
}

// This function streams an entire output to outputBuffer
void Host::StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                             Buffer* outputBuffer)
{
    int lastLayerIdx = layers.size()-1;
    // Compute each line in the outputBuffer
    for (int line = 0; line < outputBuffer->lines; line++) {

        // Update input buffers of layers recursively for the next line
        PopulateBuffers(layers, outputBuffer, lastLayerIdx, line);
        
        // Indicate that one line has been inserted into the output buffer
        outputBuffer->LineInserted();
    }

    // Free memory for input buffers of each layer
    for (auto& layer : layers) {
        layer->InputBuffer()->FreeMemory();
    }
}

void Host::MedianBlur(Buffer* outputBuffer, 
                      int kernelHeight, int kernelWidth)
{
    std::vector<std::unique_ptr<Layer>> layers;

    int bufferLines;
    if (this->camSensor->snapshot) {
        bufferLines = this->rows;
    } else {
        bufferLines = kernelHeight;
    }

    auto medianBlurLayer = std::make_unique<MedianBlurLayer>(channels, rows, cols, 
                                                             kernelHeight, kernelWidth,
                                                             bufferLines);

    auto minMaxNormLayer = std::make_unique<MinMaxNormLayer>(channels, rows, cols, 5);

    layers.push_back(std::move(medianBlurLayer));
    layers.push_back(std::move(minMaxNormLayer));

    StreamingPipeLine(layers, outputBuffer);
}


// Gaussian filter which blurs camera input and outputs an image of same size
void Host::GaussianBlur(Buffer* outputBuffer, 
                        int kernelHeight, int kernelWidth, 
                        double sigmaX, double sigmaY) 
{   
    std::vector<std::unique_ptr<Layer>> layers;

    int bufferLines;
    if (this->camSensor->snapshot) {
        bufferLines = this->rows;
    } else {
        bufferLines = kernelHeight;
    }

    auto gaussianBlurLayer = std::make_unique<GaussianBlurLayer>(channels, rows, cols, 
                                                                 kernelHeight, kernelWidth, 
                                                                 sigmaX, sigmaY,
                                                                 bufferLines);

    layers.push_back(std::move(gaussianBlurLayer));

    StreamingPipeLine(layers, outputBuffer);
}

void Host::Sobel(Buffer* outputBuffer) 
{

    std::vector<std::unique_ptr<Layer>> layers;

    int bufferLines;
    if (this->camSensor->snapshot) {
        bufferLines = this->rows;
    } else {
        bufferLines = 1;
    }

    auto sobelLayer = std::make_unique<SobelLayer>(channels, rows, cols, 
                                                   3, 3, bufferLines);

    auto minMaxNormLayer = std::make_unique<MinMaxNormLayer>(channels, rows, cols, 2);

    layers.push_back(std::move(sobelLayer));
    layers.push_back(std::move(minMaxNormLayer));
    
    StreamingPipeLine(layers, outputBuffer);
}


void Host::CannyEdge(Buffer* outputBuffer, 
                     byte lowThreshold, byte highThreshold) 
{

    std::vector<std::unique_ptr<Layer>> layers;

    int bufferLines;
    if (this->camSensor->snapshot) {
        bufferLines = this->rows;
    } else {
        bufferLines = 1;
    }

    auto grayScaleLayer = std::make_unique<GrayScaleLayer>(channels, rows, cols, 
                                                           bufferLines);

    auto gaussianBlurLayer = std::make_unique<GaussianBlurLayer>(1, rows, cols, 
                                                                 5, 5, 0, 0,
                                                                 5);

    auto sobelLayer = std::make_unique<SobelLayer>(1, rows, cols, 3, 3, 3);

    auto nmxLayer = std::make_unique<NonMaxSuppressionLayer>(1, rows, cols, 3);
    auto hysterisisLayer = std::make_unique<HysterisisLayer>(1, rows, cols, 
                                                             lowThreshold,
                                                             highThreshold, 1);

    layers.push_back(std::move(grayScaleLayer));
    layers.push_back(std::move(gaussianBlurLayer));
    layers.push_back(std::move(sobelLayer));
    layers.push_back(std::move(nmxLayer));
    layers.push_back(std::move(hysterisisLayer));

    // The first required lines for nmx's stream function exists in its input buffer

    StreamingPipeLine(layers, outputBuffer);
}
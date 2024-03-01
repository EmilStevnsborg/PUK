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
    // next line that needs to be put into layer's input buffer
    int nextLine = line + layers[currentLayerIdx]->PadHeight() + 1;

    layers[currentLayerIdx]->Stream(outputBuffer, line);
    // populate inputBuffer
    if ((line - layers[currentLayerIdx]->PadHeight() >= 0) &&
        (nextLine < layers[currentLayerIdx]->InputBuffer()->rows))
    {
        // printf("nextLine %d\n", nextLine);
        // cam sensor level
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
// The kernel is 5x5
void Host::GaussianBlur(Buffer* outputBuffer, 
                        int kernelHeight, int kernelWidth, 
                        double sigmaX, double sigmaY) 
{   
    std::vector<std::unique_ptr<Layer>> layers;

    auto gaussianBlurLayer = std::make_unique<GaussianBlurLayer>(channels, rows, cols, 
                                                                 kernelHeight, kernelWidth, 
                                                                 sigmaX, sigmaY);
    
    // stream intital lines from cam into first input
    for (int camLine = 0; camLine < gaussianBlurLayer->InputBuffer()->lines; camLine++) {
        camSensor->Stream(gaussianBlurLayer->InputBuffer(), 
                          camLine);
        gaussianBlurLayer->InputBuffer()->LineInserted();
    }

    layers.push_back(std::move(gaussianBlurLayer));

    StreamingPipeLine(layers, outputBuffer);
}




void Host::CannyEdge(Buffer* outputBuffer) {}



void Host::Sobel(Buffer* outputBuffer, 
                 int kernelHeight, int kernelWidth) 
{
    // init layers for computation
    auto sobelLayer = std::make_unique<SobelLayer>(channels, rows, cols, 
                          kernelHeight, kernelWidth);

    auto nmxLayer = std::make_unique<NonMaxSuppressionLayer>(channels, 
                                    rows, 
                                    cols,
                                    kernelHeight, 
                                    kernelWidth);

    // stream first required lines from cam into outer sobelLayer
    for (int camLine = 0; camLine < sobelLayer->InputBuffer()->lines; camLine++) {
        camSensor->Stream(sobelLayer->InputBuffer(), 
                          camLine);
        sobelLayer->InputBuffer()->LineInserted();
    }

    // stream first required lines from sobelLayer into nmxLayer
    for (int sobelLine = 0; sobelLine < nmxLayer->InputBuffer()->lines; sobelLine++) {

        // stream line from sobelLayer
        sobelLayer->Stream(nmxLayer->InputBuffer(), sobelLine);

        // sobelLayer needs new cam line
        int newCamLine =  sobelLine + sobelLayer->PadHeight() + 1;
        if ((sobelLine - sobelLayer->PadHeight() >= 0) && 
            (newCamLine < sobelLayer->InputBuffer()->rows)) 
        {
            camSensor->Stream(sobelLayer->InputBuffer(), 
                              newCamLine);
            
            sobelLayer->InputBuffer()->LineInserted();
        }
        nmxLayer->InputBuffer()->LineInserted();
    }


    std::vector<std::unique_ptr<Layer>> layers;

    layers.push_back(std::move(nmxLayer));
    layers.push_back(std::move(sobelLayer));

    // The first required lines for nmx's stream function exists in its input buffer

    StreamingPipeLine(layers, outputBuffer);
}

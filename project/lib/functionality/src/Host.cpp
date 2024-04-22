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
                           Buffer* bufferPtr,
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
    layers[currentLayerIdx]->Stream(bufferPtr, line);
}

// This function streams an entire output to buffer
void Host::StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                             Buffer* bufferPtr)
{
    int lastLayerIdx = layers.size()-1;
    // Compute each line in the buffer
    for (int line = 0; line < bufferPtr->lines; line++) {

        // Update input buffers of layers recursively for the next line
        PopulateBuffers(layers, bufferPtr, lastLayerIdx, line);
        
        // Indicate that one line has been inserted into the output buffer
        bufferPtr->LineInserted();
    }
}

// compression on top of layers (not done)
void Host::StreamingPipeLine(std::vector<std::unique_ptr<Layer>>& layers,
                             Compression* compressionPtr) 
{
    int line = 0;
    int lastLayerIdx = layers.size()-1;

    while (line < rows) {
        PopulateBuffers(layers, compressionPtr->InputBuffer(), lastLayerIdx, line);
        compressionPtr->EncodeLine(line);
        line++;
    }
}

// compressionPtr no layers
void Host::StreamingPipeLine(Compression* compressionPtr) {

    if (camSensor->snapshot) {
        camSensor->Stream(compressionPtr->InputBuffer());
        compressionPtr->InputBuffer()->lineInserts = compressionPtr->InputBuffer()->rows;
        
        for (int line = 0; line < rows; line++) {
            compressionPtr->EncodeLine(line);
        }
    } else {
        int line = 0;
        while (line < rows) {
            camSensor->Stream(compressionPtr->InputBuffer(), line);
            compressionPtr->EncodeLine(line);

            compressionPtr->InputBuffer()->LineInserted();
            line++;
        }
    }
}

// requires min-max
void Host::MedianBlur(byte* output, 
                      int kernelHeight, int kernelWidth,
                      std::string compressionType)
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

    auto minMaxNormLayer = std::make_unique<MinMaxNormLayer>(channels, rows, cols, 2);

    layers.push_back(std::move(medianBlurLayer));
    layers.push_back(std::move(minMaxNormLayer));
    
    if (compressionType == "QOI") {
        Buffer buffer(channels, rows, cols, 1);
        QOI QOIcompression(&buffer, output);
        StreamingPipeLine(layers, &QOIcompression);
    } else {
        Buffer buffer(channels, rows, cols, rows, output);
        StreamingPipeLine(layers, &buffer);
    }
}

void Host::MedianBlur(uint16_t* output, 
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

    layers.push_back(std::move(medianBlurLayer));
    
    Buffer buffer(channels, rows, cols, rows, false, output);
    StreamingPipeLine(layers, &buffer);
}


// Gaussian filter which blurs camera input and outputs an image of same size
void Host::GaussianBlur(byte* output, 
                        int kernelHeight, int kernelWidth, 
                        double sigmaX, double sigmaY,
                        std::string compressionType) 
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

    if (compressionType == "QOI") {
        Buffer buffer(channels, rows, cols, 1);
        QOI QOIcompression(&buffer, output);
        StreamingPipeLine(layers, &QOIcompression);
    } else {
        Buffer buffer(channels, rows, cols, rows, output);
        StreamingPipeLine(layers, &buffer);
    }
}

void Host::GaussianBlur(byte* output, 
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

    Buffer buffer(channels, rows, cols, rows, false, output);
    StreamingPipeLine(layers, &buffer);
}

// requires min-max
void Host::Sobel(byte* output, 
                 std::string compressionType) 
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
    
    if (compressionType == "QOI") {
        Buffer buffer(channels, rows, cols, 1);
        QOI QOIcompression(&buffer, output);
        StreamingPipeLine(layers, &QOIcompression);
    } else {
        Buffer buffer(channels, rows, cols, rows, output);
        StreamingPipeLine(layers, &buffer);
    }
}

void Host::Sobel(uint16_t* output) {
    std::vector<std::unique_ptr<Layer>> layers;

    int bufferLines;
    if (this->camSensor->snapshot) {
        bufferLines = this->rows;
    } else {
        bufferLines = 1;
    }

    auto sobelLayer = std::make_unique<SobelLayer>(channels, rows, cols, 
                                                   3, 3, bufferLines);

    layers.push_back(std::move(sobelLayer));

    Buffer buffer(channels, rows, cols, rows, true, output);
    StreamingPipeLine(layers, &buffer);
}


void Host::CannyEdge(byte* output, 
                     byte lowThreshold, byte highThreshold,
                     std::string compressionType) 
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

    if (compressionType == "QOI") {
        Buffer buffer(1, rows, cols, 1);
        QOI QOIcompression(&buffer, output);
        StreamingPipeLine(layers, &QOIcompression);
    } else {
        Buffer buffer(1, rows, cols, rows, output);
        StreamingPipeLine(layers, &buffer);
    }
}

void Host::CannyEdge(byte* output, 
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

    Buffer buffer(1, rows, cols, rows, false, output);
    StreamingPipeLine(layers, &buffer);
}

void Host::QOIencode(byte* output) {
    
    // buffer to store image input
    int bufferLines;
    if (camSensor->snapshot) {
        bufferLines = rows;
    } else {
        bufferLines = 1;
    }
    Buffer buffer(channels, rows, cols, bufferLines);
    
    QOI QOIcompression(&buffer, output);
    
    StreamingPipeLine(&QOIcompression);
}
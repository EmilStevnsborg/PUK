#ifndef NonMaxSuppressionLayer_H
#define NonMaxSuppressionLayer_H

#include "Layer.h"
#include "CVfunctions.h"

// is a successor to SobelLayer and samples an entire image of 16 bit values
class NonMaxSuppressionLayer : public Layer {
    public:
        Buffer inputBuffer;

        NonMaxSuppressionLayer(int inputChannels, 
                               int inputRows, 
                               int inputCols);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}

        std::vector<int> NextLines(int streamingLine);

        ~NonMaxSuppressionLayer() {};
};

#endif // NonMaxSuppressionLayer_H
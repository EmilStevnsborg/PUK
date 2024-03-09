#ifndef MinMaxNormLayer_H
#define MinMaxNormLayer_H

#include "Layer.h"
#include "CVfunctions.h"

// is a successor to SobelLayer in CannyEdge and stores 16 bit values
class MinMaxNormLayer : public Layer {
    public:
        Buffer inputBuffer;

        uint16_t min;
        uint16_t max;

        bool minMaxFound;

        MinMaxNormLayer(int inputChannels, 
                        int inputRows, 
                        int inputCols);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}

        void findMinMax();

        std::vector<int> NextLines(int streamingLine);

        ~MinMaxNormLayer() {};
};

#endif // MinMaxNormLayer_H
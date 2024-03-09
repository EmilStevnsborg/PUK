#ifndef HysterisisLayer_H
#define HysterisisLayer_H

#include "Layer.h"
#include "CVfunctions.h"

class HysterisisLayer : public Layer {
    public:
        Buffer inputBuffer;

        byte lowThreshold;
        byte highThreshold;

        HysterisisLayer(int inputChannels, 
                        int inputRows, 
                        int inputCols,
                        byte lowThreshold,
                        byte highThreshold);

        void Stream(Buffer* outputBuffer, int line);

        Buffer* InputBuffer() {return &this->inputBuffer;}

        std::vector<int> NextLines(int streamingLine);

        ~HysterisisLayer() {};
};

#endif // HysterisisLayer_H
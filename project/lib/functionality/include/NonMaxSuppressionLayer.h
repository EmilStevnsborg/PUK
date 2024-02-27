#ifndef NonMaxSuppressionLayer_H
#define NonMaxSuppressionLayer_H

#include "Layer.h"

class NonMaxSuppressionLayer : public Layer {
    private:
        // image specifics
        int inputChannels;
        int inputRows;
        int inputCols;
    public:
        Buffer inputBuffer;

        NonMaxSuppressionLayer(int inputChannels, int inputRows, int inputCols);

        void Stream(Buffer* outputBuffer, int line);

        ~NonMaxSuppressionLayer() {};
};

#endif // NonMaxSuppressionLayer_H
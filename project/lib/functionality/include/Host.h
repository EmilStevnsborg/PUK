#ifndef HOST_H
#define HOST_H

#include <stdint.h>
#include "Buffer.h"
#include "CVfunctions.h"
#include "Camera.h"
#include "GaussianBlurLayer.h"
#include "Layer.h"
#include <stdlib.h>
#include <stdint.h>
#include <vector>

typedef uint8_t byte;

// one camera, multiple functions (different buffering)
class Host {
    private:
        int channels;
        int rows;
        int cols;
        int bytesLine;
        Camera* camSensor; // Dependency injection
    public:
        Host(Camera* cam, int channels, int rows, int cols);
        void CannyEdge(Buffer* outputBuffer);
        void GaussianBlur(Buffer* outputBuffer);
        ~Host() {}
};

#endif // HOST_H
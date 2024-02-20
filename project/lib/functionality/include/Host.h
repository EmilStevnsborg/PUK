#include <stdint.h>
#include "Buffer.h"
#include "CVfunctions.h"
#include "Camera.h"
#include <stdlib.h>
#include <vector>

typedef uint8_t byte;

// one camera, multiple functions (different buffering)
class Host {
    private:
        int channels;
        int rows;
        int cols;
        Camera* camSensor; // Dependency injection
    public:
        Host(Camera* cam, int channels, int rows, int cols);
        void CannyEdge(int kernelHeight, int kernelWidth);
        void GaussianFilter();
};
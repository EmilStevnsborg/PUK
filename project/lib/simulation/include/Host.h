#include <stdint.h>
#include "CamSimulator.h"
#include "Buffer.h"
#include "CVfunctions.h"

typedef uint8_t byte;

// one camera, multiple functions (different buffering)
class Host {
    private:
        int channels;
        int rows;
        int cols;
        CamSimulator* camSensor; // Dependency injection
        Buffer inputBuffer; //
        Buffer outputBuffer; //
    public:
        Host(CamSimulator* camSimulator, int channels, int rows, int cols);
        void CannyEdge(int kernelHeight, int kernelWidth);
        void GaussianFilter(int kernelHeight, int kernelWidth);
}
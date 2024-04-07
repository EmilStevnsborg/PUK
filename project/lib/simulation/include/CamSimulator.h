#ifndef CAMSIMULATOR_H
#define CAMSIMULATOR_H

#include <stdint.h>
#include "Camera.h"
#include <vector>
#include <cstdio>

typedef uint8_t byte;

class CamSimulator : public Camera {
    private:
        std::vector<byte> image;
        int channels;
        int rows;
        int cols;

    public:
        CamSimulator(bool snapshot, int channels, int rows, int cols);
        
        // PROPERTIES
        std::vector<byte> GetImage();
        int Channels();
        int Rows();
        int Cols();

        // METHODS

        // Store image from file
        void StoreData(std::vector<byte> image);
        
        // line scan
        void Stream(Buffer* outputBuffer, int line);
        // snapshot
        void Stream(Buffer* outputBuffer);

        virtual ~CamSimulator() = default;

};

#endif //CAMSIMULATOR_H
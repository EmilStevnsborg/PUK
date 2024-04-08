#ifndef CameraSim_H
#define CameraSim_H

#include <stdint.h>
#include "Camera.h"
#include <vector>
#include <cstdio>

typedef uint8_t byte;

class CameraSim : public Camera {
    private:
        std::vector<byte> image;

    public:
        CameraSim(bool snapshot, int channels, int rows, int cols);
        
        // PROPERTIES
        std::vector<byte> GetImage();

        // METHODS

        // Store image from file
        void StoreData(std::vector<byte> image);
        
        // line scan
        void Stream(Buffer* outputBuffer, int line);
        // snapshot
        void Stream(Buffer* outputBuffer);

        virtual ~CameraSim() = default;

};

#endif //CameraSim_H
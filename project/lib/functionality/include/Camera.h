#ifndef CAMERA_H
#define CAMERA_H

#include "Buffer.h"

// Abstract class for camera functionality
class Camera {
    public:
        virtual void Stream(Buffer* outputMemory, int line) = 0;
        virtual void Stream(Buffer* outputMemory) = 0;
        virtual ~Camera() {}
};

#endif // CAMERA_H
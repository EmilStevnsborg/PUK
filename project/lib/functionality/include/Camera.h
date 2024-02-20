#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

typedef uint8_t byte;

// Abstract class for camera functionality
class Camera {
    public:
        virtual Camera() {};
        virtual void Stream(byte* byteArray, int line, int startIdx);
        virtual void Stream(byte* byteArray);
        // virtual ~Camera() {}
};

#endif // CAMERA_H
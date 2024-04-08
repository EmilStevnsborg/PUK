#ifndef CAMERA_H
#define CAMERA_H

#include "Buffer.h"

// Abstract class for camera functionality
class Camera {
    private:
        int channels;
        int rows;
        int cols;
    public:
        bool snapshot;

        Camera(int channels, int rows, int cols, bool snapshot) {
            this->channels = channels;
            this->rows = rows;
            this->cols = cols;
            this->snapshot = snapshot;
        }

        virtual void Stream(Buffer* outputMemory, int line) = 0;
        virtual void Stream(Buffer* outputMemory) = 0;

        int Channels() {return this->channels;}
        int Rows() {return this->rows;}
        int Cols() {return this->cols;}

        virtual ~Camera() {}
};

#endif // CAMERA_H
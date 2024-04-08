#include "CameraSim.h"
#include "util.h"

// STATIC VARIABLES

// CONSTRUCTOR

CameraSim::CameraSim(bool snapshot, int channels, int rows, int cols) :
    Camera(channels, rows, cols, snapshot) {}

// PROPERTIES
std::vector<byte> CameraSim::GetImage() {
    return image;
}

// METHODS

// Function to store image from file
void CameraSim::StoreData(std::vector<byte> image) {
    this->image = image;
}

// line scan
void CameraSim::Stream(Buffer* outputBuffer, int line) {
    // Line index within bounds
    if (line >= 0 && line < Rows()) {
        int bytesLine = Cols() * Channels();
        int startIdx = (line*bytesLine) % outputBuffer->elementsAllocated;

        fillByteArray(outputBuffer->Memory<byte>(), image, line, startIdx, bytesLine);
    }
}


// snapshot
void CameraSim::Stream(Buffer* outputBuffer) {
    // Calculate the size of the line data in bytes
    int bytes = Rows() * Cols() * Channels();

    // Start with line 0 and fill out entire bytearray
    int line = 0;
    int startIdx = 0;

    // Fill the byte array with image data
    fillByteArray(outputBuffer->Memory<byte>(), image, line, startIdx, bytes);
}

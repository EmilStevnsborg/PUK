#include "CamSimulator.h"
#include "util.h"

// STATIC VARIABLES

// CONSTRUCTOR

CamSimulator::CamSimulator(int channels, int rows, int cols) {
    this->channels = channels;
    this->rows =  rows;
    this->cols = cols;
}

// PROPERTIES
std::vector<byte> CamSimulator::GetImage() {
    return image;
}
int CamSimulator::Channels() {
    return channels;
}
int CamSimulator::Rows() {
    return rows;
}
int CamSimulator::Cols() {
    return cols;
}

// METHODS

// Function to store image from file
void CamSimulator::StoreData(std::vector<byte> image) {
    this->image = image;
}

// line scan
void CamSimulator::Stream(Buffer* outputBuffer, int line) {
    // Line index within bounds
    if (line >= 0 && line < rows) {
        int bytesLine = cols*channels;
        int startIdx = (line*bytesLine) % outputBuffer->elementsAllocated;

        fillByteArray(outputBuffer->Memory<byte>(), image, line, startIdx, bytesLine);
    }
}


// snapshot
void CamSimulator::Stream(Buffer* outputBuffer) {
    // Calculate the size of the line data in bytes
    int bytes = rows * cols * channels;

    // Start with line 0 and fill out entire bytearray
    int line = 0;
    int startIdx = 0;

    // Fill the byte array with image data
    fillByteArray(outputBuffer->Memory<byte>(), image, line, startIdx, bytes);
}

#include "CamSimulator.h"
#include "util.h"

// STATIC VARIABLES

// Define static member variables
std::optional<bool> CamSimulator::deviceExists;
bool CamSimulator::deviceClaimed = false;
int CamSimulator::deviceIdx = -1;
cv::VideoCapture CamSimulator::cap;

// CONSTRUCTOR

CamSimulator::CamSimulator(int channels, int rows, int cols) {
    this->channels = channels;
    this->rows =  rows;
    this->cols = cols;
}

// PROPERTIES
cv::Mat CamSimulator::GetData() {
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

// Static method
void CamSimulator::InitDevice() {
    // has device existence been determined?
    if (!deviceExists.has_value()) {
        // Attempt to find device index
        int maybeIdx = deviceIndex(cap);
        
        // does device exist
        if (maybeIdx == -1) {
            deviceExists = false;
        } else {
            deviceIdx = maybeIdx;
            deviceExists = true;
        }
    }
}

void CamSimulator::ClaimDevice() {
    if (!deviceExists.has_value()) {
        throw std::runtime_error("Device hasn't been initialized!");
    } else if (!deviceExists) {
        throw std::runtime_error("No capture device available");
    } else {
        if (deviceClaimed) {
            if (!deviceUsed) {
                throw std::runtime_error("Capture device used by other instance");
            }
        } else {
            // instance uses device
            deviceUsed = true;
            // an instance of this class claims the device 
            deviceClaimed = true; 
        }
    }
}

// Function to store image from file
void CamSimulator::StoreData(std::string& imagePath) {
    readImage(imagePath, this->image);
    
    // throw error
    if (image.rows != this->rows ||
        image.cols != this->cols ||
        image.channels() != this->channels) {


        printf("Stored image from path\n"
                "image.rows %d == this->rows %d\n"
                "image.cols %d == this->cols %d\n"
                "image.channels %d == this->channels %d\n", 
                image.rows, this->rows, image.cols, this->cols,
                image.channels(), this->channels);
        
        throw std::runtime_error("Ambiguity in image properties"
                                 "while storing");
    }
}

// Function to store image from cam
void CamSimulator::StoreData() {
    if (deviceClaimed && deviceUsed) {
        cap.open(deviceIdx);
        snapImage(cap, this->image);

        // throw error
        if (image.rows != this->rows ||
            image.cols != this->cols ||
            image.channels() != this->channels) {

            printf("Stored image from path\n"
                   "image.rows %d == this->rows %d\n"
                   "image.cols %d == this->cols %d\n"
                   "image.channels %d == this->channels %d\n", 
                    image.rows, this->rows, image.cols, this->cols,
                    image.channels(), this->channels);
            
            throw std::runtime_error("Ambiguity in image properties"
                                     "while storing");
        }
    } else {
        throw std::runtime_error("Instance does not own capture device");
    }
}

// line scan
void CamSimulator::Stream(byte* byteArray, int line, int startIdx) {
    // Line index within bounds
    if (line > 0 && line < rows) {
        // Calculate the size of the line data in bytes
        int bytes = cols * channels;

        // Fill byte array partially with image line starting at flat index
        fillByteArray(byteArray, image, line, startIdx, bytes);
    } else {
        std::runtime_error("Line requested out of bounds");
    }
}


// snapshot
void CamSimulator::Stream(byte* byteArray) {
    // Calculate the size of the line data in bytes
    int bytes = rows * cols * channels;

    // Start with line 0 and fill out entire bytearray
    int line = 0;
    int startIdx = 0;

    // Fill the byte array with image data
    fillByteArray(byteArray, image, line, startIdx, bytes);
}

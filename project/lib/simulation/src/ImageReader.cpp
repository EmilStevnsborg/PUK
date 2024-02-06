#include "ImageReader.h"

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// METHODS ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// SNAP AN IMAGE WITH CAM
// return the cv::MAT of the frame snapped
cv::Mat snapImage(cv::VideoCapture& cam) {
    if (!cam.isOpened()){
        throw std::invalid_argument("Cam is not opened");
    }
    cv::Mat frame;
    cam >> frame;  // Read a frame from the camera
    return frame;
}

// READ AN IMAGE FROM PATH
// return the cv::MAT of the image from the path
cv::Mat readImage(std::string& imagePath) {
    // Read the image from the specified path
    cv::Mat image = cv::imread(imagePath);
    // if no such file exists throw std::bad_function_call;
    if (image.empty()) {
        // Handle the case where the image could not be read
        throw std::invalid_argument("Path does not exist");
    }
    return image;
}

void printImg(cv::Mat& frame) {
    for(int i = 0; i < frame.rows; i++) {
        for(int j = 0; j < frame.rows; j++) {
            printf("%f", frame.at<double>(i, j));
            if (j == frame.cols-1) {printf("\n");}
        }
    }
}

// this functions as a maybe return type, in the sense it either returns a 
// nullptr `Nothing` or the actual device index `Just int`
int* camDeviceIndex(cv::VideoCapture& cam) {
    int MAX_DEVICES = 10;    
    // check which device can be used
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (cam.open(i)) {
            int* deviceIndex = new int(i);
            return deviceIndex;
        }
    }
    return nullptr;
}
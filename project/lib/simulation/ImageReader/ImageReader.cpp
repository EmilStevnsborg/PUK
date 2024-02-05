#include "ImageReader.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// CONSTRUCTOR //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ImageReader::ImageReader(const std::optional<cv::VideoCapture>& cam) {
    // If a camera is provided and not in use, use it
    if (cam && !cameraInUse) {
        this -> camera = cam.value();
        cameraInUse = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// FIELDS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ImageReader::cameraInUse = false;

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// METHODS ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// snap an image with the imageReaders camera
cv::Mat ImageReader::snapImage() {
    // if imageReader instance has a camera snap an image
    if (camera) {
        cv::Mat frame;
        (*camera) >> frame;  // Read a frame from the camera
        return frame;
    // else throw exception
    }
    throw std::bad_function_call();
}

// return the image from the path
cv::Mat ImageReader::readImage(std::string& imagePath) {
    // Read the image from the specified path
    cv::Mat image = cv::imread(imagePath);
    // if no such file exists throw std::bad_function_call;
    if (image.empty()) {
        // Handle the case where the image could not be read
        throw std::runtime_error("Error: Unable to read the image from the specified path.");
    }
    return image;
}

bool ImageReader::isCameraInUse() {
    return cameraInUse;
}

#include "util.h"

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// METHODS ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// SNAP AN IMAGE WITH CAM
// Function to capture an image from a video capture device and set it to imageRef
void snapImage(cv::VideoCapture& cap, cv::Mat& imageRef) {
    if (!cap.isOpened()){
        throw std::invalid_argument("Capture can't open (not a valid device)");
    }

    cv::Mat frame;
    cap >> frame; // Capture frame from device

    // Check if the captured frame is not empty
    if (frame.empty()) {
        throw std::runtime_error("Failed to capture frame");
    }

    imageRef = frame; // Assign the captured frame to imageRef
}


// READ AN IMAGE FROM PATH
// Function to read an image from a file path and set it to imageRef
void readImage(std::string& imagePath, cv::Mat& imageRef) {
    // Read the image from the specified path
    imageRef = cv::imread(imagePath);

    // Check if the image could not be read
    if (imageRef.empty()) {
        throw std::invalid_argument("Failed to read image from path");
    }
}

// this function returns -1 if no device available or device index
int deviceIndex(cv::VideoCapture& cap) {
    int MAX_DEVICES = 10;    
    // check which device can be used
    for (int i = 0; i < MAX_DEVICES; i++) {
        printf("Trying to open capture with device: %d\n", i);
        if (cap.open(i)) {
            printf("\nCAPTURE CAN BE OPENED WITH DEVICE: %d\n",i);
            return i;
        }
    }
    printf("\nCAPTURE COULDN'T FIND AVAILABLE DEVICE\n");
    return -1;
}

void fillByteArray(byte* byteArray, cv::Mat& imageRef, 
                   int line, int startIdx, int bytes) {

    int byteIdx = startIdx;
    int i = line;
    while ((byteIdx-startIdx) < bytes) {
        for (int j = 0; j < imageRef.cols; j++) {
            // Get the pixel
            cv::Vec3b pixel = imageRef.at<cv::Vec3b>(i, j);

            for (int c = 0; c < imageRef.channels(); c++) {
                byteArray[byteIdx++] = (byte) pixel[c];
            }
        }
        i += 1;
    }
}

void printByteArray(byte* byteArray, int channels, int rows, int cols) {
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("(");
            for (int c = 0; c < channels; c++) {
                int idx = i*cols*channels + j*channels + c;
                printf("%hhu", byteArray[idx]);
                if (c + 1 < channels) {printf(", ");}
            }
            printf(") ");
        }
        printf("\n");
    }
}

cv::Mat byteArrayToImg(byte* byteArray, int channels, int rows, int cols) {
    cv::Mat image(rows, cols, channels == 1 ? CV_8UC1 : CV_8UC3);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (channels == 1) {
                uchar intensity = byteArray[i * cols + j];
                image.at<uchar>(i, j) = intensity;
            } else {
                for (int c = 0; c < channels; c++) {
                    int idx = (i * cols + j) * channels + c;
                    image.at<cv::Vec3b>(i, j)[c] = byteArray[idx];
                }
            }
        }
    }
    return image;
}
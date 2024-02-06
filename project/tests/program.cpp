#include "program.h"
#include "ImageReader.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>

int main() {
    cv::VideoCapture cam;
    // maybe there is a camera device available
    int* maybeIdx = camDeviceIndex(cam);
    // pointer is not null
    if (maybeIdx) {
        int idx = *maybeIdx;
        cam.open(idx);
        cv::Mat frame = snapImage(cam);
        imshow("camera", frame);
        cv::waitKey();
    }
}


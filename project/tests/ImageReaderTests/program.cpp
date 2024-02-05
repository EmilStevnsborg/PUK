#include "program.h"
#include "../../lib/ImageReader/ImageReader.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

int main(){
    // cam 2 is the webcam
    cv::VideoCapture cam(2);
    ImageReader testImageReader = ImageReader(cam)
}
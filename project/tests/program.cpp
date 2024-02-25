#include "program.h"
#include "CamSimulator.h"
#include "util.h"
#include "Host.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>

int main() {
    
    // openCV tests: 3, 576, 768
    int channels = 3;
    int rows = 576;
    int cols = 768;    
    CamSimulator camSimulator(channels, rows, cols);
    std::string path = "cases/Background_Subtraction_Tutorial_frame_1.png";
    camSimulator.StoreData(path);


    // // camera tests: 3, 360, 640
    // int channels = 3;
    // int rows = 360;
    // int cols = 640;
    // CamSimulator camSimulator(channels, rows, cols);
    // CamSimulator::InitDevice();
    // camSimulator.ClaimDevice();
    // camSimulator.StoreData();

    Host host(&camSimulator, channels, rows, cols);
    Buffer outputBuffer(channels, rows, cols, rows);
    host.GaussianBlur(&outputBuffer);

    cv::Mat computedImage = byteArrayToImg(outputBuffer.memory, channels, rows, cols);

    // printByteArray(outputBuffer.memory, channels, rows, cols);

    // Display the original image
    cv::imshow("original image", camSimulator.GetImage());
    cv::waitKey(0);

    // Display the computed image
    cv::imshow("computed image", computedImage);
    cv::waitKey(0);

    outputBuffer.FreeMemory();
}


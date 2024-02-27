#include "testing.h"

bool checkCorrectness(cv::Mat& y, cv::Mat& yPrime) {
    // Check if the dimensions of the two images match
    if (y.size() != yPrime.size()) {
        return false;
    }

    // Display the predicate image
    cv::imshow("predicate image", yPrime);
    cv::waitKey(0);

    // Display the true image
    cv::imshow("true image", y);
    cv::waitKey(0);

    // Calculate absolute difference between the two images
    cv::Mat diff;
    cv::absdiff(y, yPrime, diff);

    // Show the difference image
    cv::imshow("Difference Image", diff);
    cv::waitKey(0); // Wait for a key press before closing

    return true;
}

bool gaussianBlurTest() {
    int channels = 3;
    int rows = 576;
    int cols = 768;    
    CamSimulator camSimulator(channels, rows, cols);
    std::string path = "cases/Background_Subtraction_Tutorial_frame_1.png";
    camSimulator.StoreData(path);

    int kernelHeight = 5; 
    int kernelWidth = 5;

    double sigmaX = 0;
    double sigmaY = 0;


    // predicate
    Host host(&camSimulator, channels, rows, cols);
    Buffer outputBuffer(channels, rows, cols, rows);
    host.GaussianBlur(&outputBuffer, kernelHeight, kernelWidth, sigmaX, sigmaY);
    cv::Mat yPrime = byteArrayToImg(outputBuffer.memory, channels, rows, cols);

    // true
    cv::Mat y;
    cv::GaussianBlur(camSimulator.GetImage(), y, 
                     cv::Size(kernelWidth, kernelWidth), 
                     sigmaX, sigmaY,
                     cv::BORDER_CONSTANT);

    bool result = checkCorrectness(y, yPrime);

    outputBuffer.FreeMemory();
    
    return result;
}
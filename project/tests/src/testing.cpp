#include "testing.h"

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

void checkCorrectness(cv::Mat& y) {
    // Display the predicate image
    cv::imshow("predicate image", y);
    cv::waitKey(0);
}

bool test_livecam(std::string functionType) {
    // input
    int channels = 3;
    int rows = 360;
    int cols = 640;

    std::string devicePath = "/dev/video2";
    CameraHS cameraHS(devicePath, true, channels, rows, cols);

    Host host(&cameraHS, channels, rows, cols);
    Buffer* outputBufferPtr;

    if (functionType == "gaussianBlur") {

        // int kernelHeight = 3; 
        // int kernelWidth = 3;
        // double sigmaX = 1;
        // double sigmaY = 0;

        Buffer outputBuffer(channels, rows, cols, rows, false, true);
        outputBufferPtr = &outputBuffer;

        cameraHS.Stream(&outputBuffer);

        cv::Mat image(rows, cols, CV_8UC(channels), outputBuffer.Memory<byte>());

        checkCorrectness(image);
    }

    outputBufferPtr->FreeMemory();

    return true;
}

bool test_simulator(std::string functionType) {
    // input
    int channels = 3;
    int rows = 576;
    int cols = 768;
    CamSimulator camSimulator(true, channels, rows, cols);
    std::string path = "cases/Background_Subtraction_Tutorial_frame_1.png";
    cv::Mat inputImage = cv::imread(path);

    std::vector<byte> vectorInputImage;

    for (int i = 0; i < inputImage.rows; i++) {
        for (int j = 0; j < inputImage.cols; j++) {
            cv::Vec3b pixel = inputImage.at<cv::Vec3b>(i,j);
            for (int c = 0; c < inputImage.channels(); c++) {
                vectorInputImage.push_back((byte) pixel[c]);
            }
        }
    }

    camSimulator.StoreData(vectorInputImage);

    Host host(&camSimulator, channels, rows, cols);


    // optimized functionality output
    cv::Mat hostOutput;
    Buffer* outputBufferPtr;
    
    if (functionType == "sobel") {

        Buffer outputBuffer(1, rows, cols, rows, false, true);
        outputBufferPtr = &outputBuffer;

        host.Sobel(&outputBuffer);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);
    } 
    else if (functionType == "gaussianBlur") {

        int kernelHeight = 3; 
        int kernelWidth = 3;
        double sigmaX = 1;
        double sigmaY = 0;

        Buffer outputBuffer(channels, rows, cols, rows, false, true);
        outputBufferPtr = &outputBuffer;
        
        host.GaussianBlur(&outputBuffer, kernelHeight, kernelWidth, sigmaX, sigmaY);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), channels, rows, cols);

    }
    else if (functionType == "cannyEdge") {

        uint16_t lowThreshold = 100;
        uint16_t highThreshold = 200;

        Buffer outputBuffer(1, rows, cols, rows, true, true);
        outputBufferPtr = &outputBuffer;
        
        host.CannyEdge(&outputBuffer, lowThreshold, highThreshold);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);
    }
    else if (functionType == "cannyEdgeManual") {

        uint16_t lowThreshold = 50;
        uint16_t highThreshold = 100;

        Buffer outputBuffer(1, rows, cols, rows, true, true);
        outputBufferPtr = &outputBuffer;
        
        host.CannyEdge(&outputBuffer, lowThreshold, highThreshold);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);
    }

    checkCorrectness(hostOutput);

    outputBufferPtr->FreeMemory();
    
    return false;
}
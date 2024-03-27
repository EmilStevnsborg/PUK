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

    double absSum = cv::sum(diff)[0];

    double minVal, maxVal;
    cv::minMaxLoc(diff.reshape(1), &minVal, &maxVal);

    printf("diff abs sum: %f\n", absSum);
    printf("diff max %f\n", maxVal);

    // Show the difference image
    cv::imshow("Difference Image", diff);
    cv::waitKey(0); // Wait for a key press before closing

    return true;
}

bool test(std::string functionType) {
    // input
    int channels = 3;
    int rows = 576;
    int cols = 768;
    CamSimulator camSimulator(channels, rows, cols);
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
    // cv implementation output
    cv::Mat cvOutput;
    
    if (functionType == "sobel") {

        int kernelSize = 3;

        Buffer outputBuffer(1, rows, cols, rows, false, true);
        outputBufferPtr = &outputBuffer;

        host.Sobel(&outputBuffer);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);

        cv::Mat gray;
        cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
        cvOutput = minMaxNorm(std::get<0>(sobel(gray, kernelSize)));
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

        cvOutput = gaussianBlur(inputImage, 
                                kernelHeight, kernelWidth, 
                                sigmaX, sigmaY);
    }
    else if (functionType == "cannyEdge") {

        uint16_t lowThreshold = 100;
        uint16_t highThreshold = 200;

        Buffer outputBuffer(1, rows, cols, rows, true, true);
        outputBufferPtr = &outputBuffer;
        
        host.CannyEdge(&outputBuffer, lowThreshold, highThreshold);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);

        cv::Mat cannyOutput = cannyEdge(inputImage, 
                                        lowThreshold, highThreshold);
        
        cvOutput = cannyOutput;
    }
    else if (functionType == "cannyEdgeManual") {

        uint16_t lowThreshold = 50;
        uint16_t highThreshold = 100;

        Buffer outputBuffer(1, rows, cols, rows, true, true);
        outputBufferPtr = &outputBuffer;
        
        host.CannyEdge(&outputBuffer, lowThreshold, highThreshold);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);

        cvOutput = cannyEdgeManual(inputImage, lowThreshold, highThreshold);
    }

    checkCorrectness(cvOutput, hostOutput);

    outputBufferPtr->FreeMemory();
    
    return false;
}
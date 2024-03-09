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
    camSimulator.StoreData(path);


    Host host(&camSimulator, channels, rows, cols);


    // optimized functionality output
    cv::Mat hostOutput;
    Buffer* outputBufferPtr;
    // cv implementation output
    cv::Mat cvOutput;
    
    if (functionType == "sobel") {

        int kernelSize = 3;

        Buffer outputBuffer(1, rows, cols, rows, true, true);
        outputBufferPtr = &outputBuffer;

        host.Sobel(&outputBuffer);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);

        cv::Mat gray;
        cv::cvtColor(camSimulator.GetImage(), gray, cv::COLOR_BGR2GRAY);

        cvOutput = sobel(gray, kernelSize).first;
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

        cvOutput = gaussianBlur(camSimulator.GetImage(), 
                                kernelHeight, kernelWidth, 
                                sigmaX, sigmaY);
    }
    else if (functionType == "cannyEdge") {

        byte lowThreshold = 100;
        byte highThreshold = 200;

        Buffer outputBuffer(1, rows, cols, rows, true, true);
        outputBufferPtr = &outputBuffer;
        
        host.CannyEdge(&outputBuffer, lowThreshold, highThreshold);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);

        cvOutput = cannyEdge(camSimulator.GetImage(), lowThreshold, highThreshold);
    }
    else if (functionType == "cannyEdgeManual") {

        byte lowThreshold = 100;
        byte highThreshold = 200;

        Buffer outputBuffer(1, rows, cols, rows, true, true);
        outputBufferPtr = &outputBuffer;
        
        host.CannyEdge(&outputBuffer, lowThreshold, highThreshold);
        hostOutput = byteArrayToImg(outputBuffer.Memory<byte>(), 1, rows, cols);

        cvOutput = cannyEdgeManual(camSimulator.GetImage(), lowThreshold, highThreshold);
    }

    checkCorrectness(cvOutput, hostOutput);

    outputBufferPtr->FreeMemory();
    
    return false;
}
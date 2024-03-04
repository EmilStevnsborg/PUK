#include "functionality.h"

cv::Mat gaussianBlur(cv::Mat img,
                     int kernelHeight, int kernelWidth, 
                     int sigmaX, int sigmaY) {
    
    cv::Mat cvOutput;
    cv::GaussianBlur(img, cvOutput, 
                     cv::Size(kernelWidth, kernelWidth), 
                     sigmaX, sigmaY,
                     cv::BORDER_CONSTANT);
    return cvOutput;
}

// returns only the gradient magnitude
cv::Mat sobel(cv::Mat img, int ksize) {
    cv::Mat cvOutput;

    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;
    cv::Sobel(img, grad_x, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT );
    cv::Sobel(img, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT );
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, cvOutput);

    return cvOutput;
}

cv::Mat cannyEdge(cv::Mat img, int lowThreshold, int highThreshold) {
    cv::Mat cvOutput;
    cv::Canny(img, cvOutput, lowThreshold, highThreshold);

    return cvOutput;
}
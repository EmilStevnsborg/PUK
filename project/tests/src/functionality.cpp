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
std::pair<cv::Mat, cv::Mat> sobel(cv::Mat img, int ksize) {
    
    int ddepth = CV_64F;

    cv::Mat grad_x, grad_y;
    cv::Sobel(img, grad_x, ddepth, 1, 0, ksize);
    cv::Sobel(img, grad_y, ddepth, 0, 1, ksize);
    
    cv::Mat magnitude, direction;
    cartToPolar(grad_x, grad_y, magnitude, direction, true);

    double minVal, maxVal;
    minMaxLoc(magnitude, &minVal, &maxVal); // Find min and max values in gradient magnitude
    magnitude = (magnitude * 255 / maxVal);

    magnitude.convertTo(magnitude, CV_8U);

    direction *= 180.0 / M_PI;
    direction.convertTo(direction, CV_8U);

    return std::make_pair(magnitude, direction);
}

cv::Mat nonMaxSuppression(cv::Mat& magnitude, cv::Mat& direction) {
    cv::Mat nonMaxSuppressed = magnitude.clone();

    for (int i = 1; i < magnitude.rows - 1; ++i) {
        for (int j = 1; j < magnitude.cols - 1; ++j) {
            float currentGradientDirection = direction.at<uchar>(i - 1, j - 1);
            if (currentGradientDirection < 0) {currentGradientDirection += 180;}

            float pixel1, pixel2;

            // Horizontal edge
            if (currentGradientDirection < 22.5 || currentGradientDirection >= 157.5) {
                pixel1 = magnitude.at<uchar>(i, j - 1);
                pixel2 = magnitude.at<uchar>(i, j + 1);
            }
            // Diagonal edge
            else if (currentGradientDirection >= 22.5 && currentGradientDirection < 67.5) {
                pixel1 = magnitude.at<uchar>(i - 1, j - 1);
                pixel2 = magnitude.at<uchar>(i + 1, j + 1);
            }
            // Vertical edge
            else if (currentGradientDirection >= 67.5 && currentGradientDirection < 112.5) {
                pixel1 = magnitude.at<uchar>(i - 1, j);
                pixel2 = magnitude.at<uchar>(i + 1, j);
            }
            // Anti-diagonal edge
            else {
                pixel1 = magnitude.at<uchar>(i - 1, j + 1);
                pixel2 = magnitude.at<uchar>(i + 1, j - 1);
            }

            // Suppress non-maximum pixels
            if (magnitude.at<uchar>(i, j) < pixel1 || magnitude.at<uchar>(i, j) < pixel2) {
                nonMaxSuppressed.at<uchar>(i, j) = 0;
            }
        }
    }

    return nonMaxSuppressed;
}

cv::Mat cannyEdge(cv::Mat img, int lowThreshold, int highThreshold) {
    cv::Mat cvOutput;
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, cvOutput, lowThreshold, highThreshold);

    return cvOutput;
}

cv::Mat cannyEdgeManual(cv::Mat img, uint16_t lowThreshold, uint16_t highThreshold) {
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat gaussOutput = gaussianBlur(gray, 5, 5, 0, 0);

    std::pair<cv::Mat, cv::Mat> sobelOutput = sobel(gaussOutput, 3);
    cv::Mat gradMagnitude = sobelOutput.first;
    cv::Mat gradDirection = sobelOutput.second;

    cv::Mat cvOutput = nonMaxSuppression(gradMagnitude, gradDirection);;
    return cvOutput;
}
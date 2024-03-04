#ifndef FUNCTIONALITY_H
#define FUNCTIONALITY_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

cv::Mat sobel(cv::Mat img, int ksize);

cv::Mat gaussianBlur(cv::Mat img,
                     int kernelHeight, int kernelWidth, 
                     int sigmaX, int sigmaY);

cv::Mat cannyEdge(cv::Mat img, int lowThreshold, int highThreshold);

#endif // FUNCTIONALITY_H
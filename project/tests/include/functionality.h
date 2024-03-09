#ifndef FUNCTIONALITY_H
#define FUNCTIONALITY_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

std::pair<cv::Mat, cv::Mat> sobel(cv::Mat img, int ksize);

cv::Mat nonMaxSuppression(cv::Mat& magnitude, cv::Mat& direction);

cv::Mat gaussianBlur(cv::Mat img,
                     int kernelHeight, int kernelWidth, 
                     int sigmaX, int sigmaY);

cv::Mat cannyEdgeManual(cv::Mat img, uint16_t lowThreshold, uint16_t highThreshold);

cv::Mat cannyEdge(cv::Mat img, int lowThreshold, int highThreshold);

#endif // FUNCTIONALITY_H
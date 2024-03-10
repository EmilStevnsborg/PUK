#ifndef FUNCTIONALITY_H
#define FUNCTIONALITY_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

typedef uint8_t byte;

std::tuple<cv::Mat, cv::Mat> sobel(cv::Mat img, int ksize);

cv::Mat minMaxNorm(cv::Mat magnitude);

cv::Mat nonMaxSuppression(cv::Mat magnitude, cv::Mat direction);

cv::Mat gaussianBlur(cv::Mat img,
                     int kernelHeight, int kernelWidth, 
                     int sigmaX, int sigmaY);

cv::Mat cannyEdge(cv::Mat img, uint16_t lowThreshold, uint16_t highThreshold);

cv::Mat cannyEdgeManual(cv::Mat img, uint16_t lowThreshold, uint16_t highThreshold);

#endif // FUNCTIONALITY_H
// ImageReader.h
#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>

cv::Mat snapImage(cv::VideoCapture& cam);
cv::Mat readImage(std::string& imagePath);
void printImg(cv::Mat& frame);
int* camDeviceIndex(cv::VideoCapture& cam);

#endif // IMAGEREADER_H

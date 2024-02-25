#ifndef UTIL_H
#define UTIL_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <stdint.h>

typedef uint8_t byte;

// snap image captured by cap into the intialized imageRef object
void snapImage(cv::VideoCapture& cap, cv::Mat& imageRef);
// read image from path into the intialized imageRef object
void readImage(std::string& imagePath, cv::Mat& imageRef);
int deviceIndex(cv::VideoCapture& cap);
void fillByteArray(byte* byteArray, cv::Mat& imageRef, 
                   int line, int startIdx, int bytes);
void printByteArray(byte* byteArray, int channels, int rows, int cols);
cv::Mat byteArrayToImg(byte* byteArray, int channels, int rows, int cols);

#endif // UTIL_H

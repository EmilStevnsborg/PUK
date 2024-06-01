#ifndef TESTING_H
#define TESTING_H

#include "CameraSim.h"
#include "CameraHS.h"
#include "util.h"
#include "Host.h"
#include <optional>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>
#include <chrono>
#include <fstream>

cv::Mat byteArrayToImg(byte* byteArray, int channels, int rows, int cols);

cv::Mat uint16ArrayToImg(uint16_t* byteArray, int channels, int rows, int cols);

bool checkCorrectness(cv::Mat& y, cv::Mat& yPrime);

CameraHS makeCameraHS();

CameraSim makeCameraSim(bool snapshot);

void test(bool hasDevice, bool snapshot, 
          std::string functionType, std::string compressionType = "");

void performanceTest(std::string functionType, std::string compression);

void memoryTestEmbedded(std::string functionType);

void memoryTestOCV(std::string functionType);

void function(Host& host, 
              std::string functionType, 
              std::string compressionType = "");

#endif // TESTING_H
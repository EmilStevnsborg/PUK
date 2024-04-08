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

cv::Mat byteArrayToImg(byte* byteArray, int channels, int rows, int cols);

bool checkCorrectness(cv::Mat& y, cv::Mat& yPrime);

CameraHS makeCameraHS();

CameraSim makeCameraSim(bool snapshot);

void test(std::string functionType, bool hasDevice, bool snapshot);

void function(std::string functionType, Host& host);

#endif // TESTING_H
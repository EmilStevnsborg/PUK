#ifndef TESTING_H
#define TESTING_H

#include "CamSimulator.h"
#include "util.h"
#include "Host.h"
#include "functionality.h"
#include <optional>
#include <vector>

cv::Mat byteArrayToImg(byte* byteArray, int channels, int rows, int cols);

bool checkCorrectness(cv::Mat& y, cv::Mat& yPrime);

bool test(std::string functionType);

#endif // TESTING_H
#ifndef TESTING_H
#define TESTING_H

#include "CamSimulator.h"
#include "CameraHS.h"
#include "util.h"
#include "Host.h"
#include <optional>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vector>

cv::Mat byteArrayToImg(byte* byteArray, int channels, int rows, int cols);

bool checkCorrectness(cv::Mat& y, cv::Mat& yPrime);

bool test_livecam(std::string functionType);

bool test_simulator(std::string functionType);

#endif // TESTING_H
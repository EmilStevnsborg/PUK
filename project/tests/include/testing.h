#ifndef TESTING_H
#define TESTING_H

#include "CamSimulator.h"
#include "util.h"
#include "Host.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>

bool checkCorrectness(cv::Mat& y, cv::Mat& yPrime);

bool gaussianBlurTest();

#endif // TESTING_H
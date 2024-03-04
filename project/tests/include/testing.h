#ifndef TESTING_H
#define TESTING_H

#include "CamSimulator.h"
#include "util.h"
#include "Host.h"
#include "functionality.h"
#include <optional>

bool checkCorrectness(cv::Mat& y, cv::Mat& yPrime);

bool test(std::string functionType);

#endif // TESTING_H
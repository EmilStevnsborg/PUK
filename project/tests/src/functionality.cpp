#include "functionality.h"

cv::Mat gaussianBlur(cv::Mat img,
                     int kernelHeight, int kernelWidth, 
                     int sigmaX, int sigmaY) {
    
    cv::Mat cvOutput;
    cv::GaussianBlur(img, cvOutput, 
                     cv::Size(kernelWidth, kernelWidth), 
                     sigmaX, sigmaY,
                     cv::BORDER_CONSTANT);
    return cvOutput;
}

// returns only the gradient magnitude
std::tuple<cv::Mat, cv::Mat> sobel(cv::Mat img, int ksize) {
    
    int ddepth = CV_64F;

    cv::Mat grad_x, grad_y;
    cv::Sobel(img, grad_x, ddepth, 1, 0, ksize);
    cv::Sobel(img, grad_y, ddepth, 0, 1, ksize);
    
    cv::Mat magnitude, direction;
    cartToPolar(grad_x, grad_y, magnitude, direction, true);

    direction *= 180.0 / M_PI;

    return std::make_tuple(magnitude, direction);
}

cv::Mat minMaxNorm(cv::Mat magnitude) {
    double minVal, maxVal;
    minMaxLoc(magnitude, &minVal, &maxVal); // Find min and max values

    cv::Mat scaledMagnitude = (magnitude * 255 / maxVal);

    scaledMagnitude.convertTo(scaledMagnitude, CV_8U);

    return scaledMagnitude;
}

cv::Mat nonMaxSuppression(cv::Mat magnitude, cv::Mat direction) {
    cv::Mat result = magnitude.clone();

    for (int i = 1; i < magnitude.rows - 1; ++i) {
        for (int j = 1; j < magnitude.cols - 1; ++j) {
            double currentGradientDirection = direction.at<double>(i - 1, j - 1);
            if (currentGradientDirection < 0) {currentGradientDirection += 180;}

            double pixel1, pixel2;

            // up-down angle (horizontal edge)
            if (currentGradientDirection < 22.5 || currentGradientDirection >= 157.5) {
                pixel1 = magnitude.at<double>(i, j - 1);
                pixel2 = magnitude.at<double>(i, j + 1);
            }
            // diagonal up angle (diagonal down edge)
            else if (currentGradientDirection >= 22.5 && currentGradientDirection < 67.5) {
                pixel1 = magnitude.at<double>(i + 1, j - 1);
                pixel2 = magnitude.at<double>(i - 1, j + 1);
            }
            // horizontal angle (up-down edge)
            else if (currentGradientDirection >= 67.5 && currentGradientDirection < 112.5) {
                pixel1 = magnitude.at<double>(i - 1, j);
                pixel2 = magnitude.at<double>(i + 1, j);
            }
            // diagonal down angle (diagonal down edge)
            else {
                pixel1 = magnitude.at<double>(i - 1, j - 1);
                pixel2 = magnitude.at<double>(i + 1, j + 1);
            }

            // Suppress non-maximum pixels
            if (magnitude.at<double>(i, j) < pixel1 || magnitude.at<double>(i, j) < pixel2) {
                result.at<double>(i, j) = 0;
            }
        }
    }

    return result;
}

cv::Mat hysteresis(cv::Mat img, uint16_t lowThreshold, uint16_t highThreshold) {
    cv::Mat result = img.clone();

    for (int i = 1; i < img.rows - 1; ++i) {
        for (int j = 1; j < img.cols - 1; ++j) {
            if (img.at<double>(i, j) >= highThreshold) {
                result.at<double>(i, j) = 255; // Strong edge pixel
            } else if (img.at<double>(i, j) >= lowThreshold) {
                // Check neighboring pixels
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (img.at<double>(i + di, j + dj) >= highThreshold) {
                            img.at<double>(i,j) = 255;
                            result.at<double>(i, j) = 255; // Weak edge pixel connected to strong edge
                            break;
                        } else {
                            result.at<double>(i, j) = 0; // Not an edge pixel
                        }
                    }
                }
            } else {
                img.at<double>(i,j) = 0;
                result.at<double>(i, j) = 0; // Not an edge pixel
            }
        }
    }

    return result;
}

cv::Mat cannyEdgeManual(cv::Mat img, uint16_t lowThreshold, uint16_t highThreshold) {
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat gaussOutput = gaussianBlur(gray, 5, 5, 0, 0);

    std::tuple<cv::Mat, cv::Mat> sobelOutput = sobel(gaussOutput, 3);
    cv::Mat gradMagnitude = std::get<0>(sobelOutput);
    cv::Mat gradDirection = std::get<1>(sobelOutput);

    cv::Mat nmxsOutput = nonMaxSuppression(gradMagnitude, gradDirection);
    cv::Mat hysterisOutput = hysteresis(nmxsOutput, lowThreshold, highThreshold);

    hysterisOutput.convertTo(hysterisOutput, CV_8U);
    return hysterisOutput;
}

cv::Mat cannyEdge(cv::Mat img, uint16_t lowThreshold, uint16_t highThreshold) {

    // actual computation
    cv::Mat cvOutput;
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, cvOutput, lowThreshold, highThreshold);

    return cvOutput;
}
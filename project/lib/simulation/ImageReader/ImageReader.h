// ImageReader.h
#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>

class ImageReader {
    public:
        ImageReader(const std::optional<cv::VideoCapture>& cam = std::nullopt);
        cv::Mat snapImage();
        cv::Mat readImage(std::string& imagePath);
        static bool isCameraInUse();
    private:
        cv::VideoCapture camera;
        static bool cameraInUse;
};

#endif // IMAGEREADER_H

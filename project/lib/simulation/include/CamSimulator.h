#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>
#include <stdint.h>
#include "Camera.h"

typedef uint8_t byte;

class CamSimulator : public Camera {
private:

    // BEFORE READING DATA

    static std::optional<bool> deviceExists;
    static bool deviceClaimed;
    static int deviceIdx; // Index in `/dev/video*`
    static cv::VideoCapture cap; // Cap instance shared by all instances

    bool deviceUsed; // Is device used by current instance

    // AFTER READING DATA

    cv::Mat image;

    int channels;
    int rows;
    int cols;

public:
    CamSimulator(int channels, int rows, int cols);

    // PROPERTIES
    cv::Mat GetData();
    int Channels();
    int Rows();
    int Cols();

    // METHODS

    static void InitDevice();
    void ClaimDevice();

    // Store image from file
    void StoreData(std::string& imagePath);
    // Store image from cam
    void StoreData();
    
    // line scan
    void Stream(byte* byteArray, int line, int startIdx);
    // snapshot
    void Stream(byte* byteArray);

    // virtual ~CamSimulator() = default;

};
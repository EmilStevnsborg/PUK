#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>
#include <stdint.h>

typedef uint8_t byte;

class CamSimulator {
private:

    // BEFORE READING DATA

    static std::optional<bool> deviceExists;
    static bool deviceClaimed;
    static int deviceIdx; // Index in `/dev/video*`
    static cv::VideoCapture cap; // Cap instance shared by all instances

    bool deviceUsed; // Is device used by current instance

    // AFTER READING DATA

    cv::Mat image;

    int channels = 0;
    int rows = 0;
    int cols = 0;

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
    void Stream(byte* byteArray, int bytesAllocated, int line);
    // snapshot
    void Stream(byte* byteArray, int bytesAllocated);

};
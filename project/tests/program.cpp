#include "program.h"
#include "CamSimulator.h"
#include "util.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>

int main() {
    // // openCV tests: 3, 576, 768
    // CamSimulator camSimulator(3, 576, 768);
    // std::string path = "cases/Background_Subtraction_Tutorial_frame_1.png";
    // camSimulator.StoreData(path);
    
    // camera tests: 3, 360, 640
    CamSimulator camSimulator(3, 360, 640);
    CamSimulator::InitDevice();
    camSimulator.ClaimDevice();
    camSimulator.StoreData();

    int line = 1;
    int cols = camSimulator.Cols();
    int channels = camSimulator.Channels();
    
    // number of bytes to allocate
    size_t bytes = cols*channels;

    // fix this
    byte*ptr = (byte*) malloc(bytes*sizeof(byte));
    
    camSimulator.Stream(ptr, bytes, line);
    cv::Mat image = camSimulator.GetData();
    // cv::imshow("camera", image);
    // cv::waitKey();
    
    for (int j = 0; j < cols; j++) {
        cv::Vec3b pixel = image.at<cv::Vec3b>(line, j);
        for (int c = 0; c < channels; c++) {
            byte stream = ptr[j*channels+c];
            byte orig = (byte) pixel[c];
            if (stream != orig) {
                printf("Failure j %d, c %d : %d == %d\n", j, c, stream, orig);
                free(ptr);
                return 0;
            }
        }
    }
    free(ptr);
    return 0;
}


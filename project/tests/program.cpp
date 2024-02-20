#include "program.h"
#include "CamSimulator.h"
#include "util.h"
#include "Host.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <optional>

int main() {
    // openCV tests: 3, 576, 768
    int channels = 3;
    int rows = 768;
    int cols = 576;
    CamSimulator camSimulator(channels, cols, rows);
    std::string path = "cases/Background_Subtraction_Tutorial_frame_1.png";
    camSimulator.StoreData(path);
    
    // // camera tests: 3, 360, 640
    // CamSimulator camSimulator(3, 360, 640);
    // CamSimulator::InitDevice();
    // camSimulator.ClaimDevice();
    // camSimulator.StoreData();

    Host host(&camSimulator, channels, rows, cols);
    host.GaussianFilter();
}


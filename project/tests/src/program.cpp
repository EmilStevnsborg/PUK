#include "program.h"

int main() {

    // test("sobel");
    // test_simulator("gaussianBlur");
    // test("cannyEdge");
    // test("cannyEdgeManual");

    // // camera tests: 3, 360, 640
    test_livecam("gaussianBlur");
    // int channels = 3;
    // int rows = 360;
    // int cols = 640;
    // CamSimulator camSimulator(channels, rows, cols);
    // CamSimulator::InitDevice();
    // camSimulator.ClaimDevice();
    // camSimulator.StoreData();
}


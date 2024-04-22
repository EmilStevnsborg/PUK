#include "program.h"

int main() {
    bool hasDevice = false;
    bool snapshot = true;

    // std::string functionType = "gaussianBlur";
    // std::string functionType = "cannyEdge";
    std::string functionType = "sobel";
    // std::string functionType = "";
    std::string compressionType = "";
    // std::string compressionType = "QOI";

    test(hasDevice, snapshot, functionType, compressionType);
}


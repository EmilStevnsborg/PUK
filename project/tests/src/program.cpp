#include "program.h"

int main() {
    bool performance_test = true;
    bool memory_test = false;
    bool memory_test_embedded = true;

    bool hasDevice = false;
    bool snapshot = true;

    // std::string functionType = "gaussianBlur";
    // std::string functionType = "medianBlur";
    std::string functionType = "cannyEdge";
    // std::string functionType = "sobel";
    // std::string functionType = "";
    // std::string compressionType = "";
    std::string compressionType = "QOI";

    if (memory_test) {
        if (memory_test_embedded) {
            memoryTestEmbedded(functionType);
        } else {
            memoryTestOCV(functionType);
        }
    } else {
        if (performance_test) {
            performanceTest(functionType, compressionType);
        } else {
            test(hasDevice, snapshot, functionType, compressionType);
        }
    }
}

// valgrind --tool=massif --massif-out-file=massif.out ./program
// massif-visualizer massif.out
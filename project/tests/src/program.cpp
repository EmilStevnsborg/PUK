#include "program.h"

int main() {
    bool hasDevice = false;
    bool snapshot = true;

    test("QOIencode", hasDevice, snapshot);
    // test("gaussianBlur", hasDevice, snapshot);
    // test("sobel", hasDevice, snapshot);
    // test("cannyEdge", hasDevice, snapshot);
}


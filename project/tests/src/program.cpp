#include "program.h"

int main() {
    bool hasDevice = true;
    bool snapshot = false;

    // test("sobel", hasDevice, snapshot);
    // test("gaussianBlur", hasDevice, snapshot);
    test("cannyEdge", hasDevice, snapshot);
}


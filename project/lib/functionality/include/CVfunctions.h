#ifndef CVFUNCTIONS_H
#define CVFUNCTIONS_H

#include "Buffer.h"

#include <stdint.h>
#include <stdlib.h>
#include <vector>

typedef uint8_t byte;

void Convolution(Buffer* inputBuffer, Buffer* outputBuffer,
                 int& outMemIdx,
                 std::vector<std::vector<float>>& kernel,
                 int& kernelHeight, int& kernelWidth,
                 int& startLine, int& startCol, // padding embedded
                 int& channels, int& cols);

#endif // CVFUNCTIONS_H
#ifndef CVFUNCTIONS_H
#define CVFUNCTIONS_H

#include "Buffer.h"

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <cstdio>
#include <cmath>

typedef uint8_t byte;

template<typename KernelType>
float Convolution(Buffer* inputBuffer,
                  std::vector<std::vector<KernelType>>& kernel,
                  int& kernelHeight, int& kernelWidth,
                  int& startLine, int& startCol, int c,
                  int& channels, int& cols);

bool Hysterisis(Buffer* inputBuffer, 
                int& i, int& j,
                int& rows, int& cols);

#endif // CVFUNCTIONS_H
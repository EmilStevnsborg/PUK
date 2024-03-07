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
float MatMul(Buffer* inputBuffer,
             std::vector<std::vector<KernelType>>& kernel,
             int& kernelHeight, int& kernelWidth,
             int& ai, int& aj, int& c);

bool Hysterisis(Buffer* inputBuffer,
                int& ai, int& aj, int& c);

#endif // CVFUNCTIONS_H
#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <vector>
#include <cstdio>

typedef uint8_t byte;

void fillByteArray(byte* byteArray, std::vector<byte>& imageRef, 
                   int line, int startIdx, int bytes);
void printByteArray(byte* byteArray, int channels, int rows, int cols);

#endif // UTIL_H

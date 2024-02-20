#include <stdint.h>
#include <vector>

typedef uint8_t byte;

void Convolution(byte* inputBuffer, byte* outputBuffer, 
                 std::vector<std::vector<float>>& kernel, 
                 int& kernelHeight, int& kernelWidth,
                 int& si, int& sj, int& inputStartLine, int& outputLine,
                 int& channels, int& cols, int& rows, 
                 int& padHeight, int& padWidth,
                 int& strideHeight, int& strideWidth);
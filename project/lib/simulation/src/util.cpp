#include "util.h"

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// METHODS ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void fillByteArray(byte* byteArray, std::vector<byte>& imageRef, 
                   int line, int startIdx, int bytes) {

    int byteIdx = startIdx;
    while ((byteIdx-startIdx) < bytes) {
        byteArray[byteIdx] = imageRef[line*bytes+(byteIdx-startIdx)];
        byteIdx += 1;
    }
}

void printByteArray(byte* byteArray, int channels, int rows, int cols) {
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("(");
            for (int c = 0; c < channels; c++) {
                int idx = i*cols*channels + j*channels + c;
                printf("%hhu", byteArray[idx]);
                if (c + 1 < channels) {printf(", ");}
            }
            printf(") ");
        }
        printf("\n");
    }
}
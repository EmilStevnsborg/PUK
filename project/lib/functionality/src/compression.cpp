#include "compression.h"

// Pixel
Pixel::Pixel(int channels) : data(channels) {}

Pixel::Pixel(std::vector<byte> data) {this->data = data;}

Pixel::Pixel(int channels, int index,  byte* memory) 
    : data(channels)
{
    for (int i = 0; i < channels; i++) {
        this->data[i] = memory[index+i];
    }
}

std::vector<byte>& Pixel::Data() {return this->data;}

// Implementation of equality operator overload
bool Pixel::operator==(const Pixel& other) const {
    return this->data == other.data;
}

std::vector<int> Pixel::operator-(const Pixel& other) const {
    std::vector<int> result(data.size());
    for (unsigned int i = 0; i < data.size(); i++) {
        int difference = ((int) data[i]) - ((int) other.data[i]);
        result[i] = difference;
    }
    return result;
}


// QOI

QOI::QOI(Buffer* bufferPtr, byte* byteArray) 
    : seenPixels(64, Pixel(bufferPtr->channels)),
      prevPixel(Pixel(bufferPtr->channels))
{
    this->bufferPtr = bufferPtr;
    this->byteArray = byteArray;
    this->byteArrayIdx = 0;
    this->lastPixelIdx = bufferPtr->channels * 
                         bufferPtr->rows * 
                         bufferPtr->rows - bufferPtr->channels;

    this->run = 0;

    QOIheader();
}

Buffer* QOI::InputBuffer() {
    return this->bufferPtr;
}

void QOI::QOIheader() {
    this->byteArray[byteArrayIdx++] = 'q';
    this->byteArray[byteArrayIdx++] = 'o';
    this->byteArray[byteArrayIdx++] = 'i';
    this->byteArray[byteArrayIdx++] = 'f';

    int32toBytes(bufferPtr->rows, byteArray, byteArrayIdx);
    int32toBytes(bufferPtr->cols, byteArray, byteArrayIdx);
    
    byteArray[byteArrayIdx++] = bufferPtr->channels;

    // sRGB??
    byteArray[byteArrayIdx++] = 1;
}

// Encode the line called for
void QOI::EncodeLine(int& line) {
    //
    int lastJ = bufferPtr->lineSize - bufferPtr->channels;
    int increment = bufferPtr->channels;
    
    for (int j = 0; j <= lastJ; j+=increment) {
        
        int index = bufferPtr->LineMemoryIndex(line) + j;
        Pixel currPixel(bufferPtr->channels, index, bufferPtr->Memory<byte>());
        
        if (this->prevPixel == currPixel) {
            this->run++;

            if (run == 62 || index == this->lastPixelIdx) {
                int runLength = run-1;
                byteArray[byteArrayIdx++] = QOI_OP_RUN + runLength;
                run = 0;
            }
        } else {
            if (run > 0) {
                int runLength = run-1;
                byteArray[byteArrayIdx++] = QOI_OP_RUN + runLength;
                run = 0;
            }
            byte hash = hashFunction(currPixel);
            
            if (this->prevPixel == currPixel) {
                byteArray[byteArrayIdx++] = QOI_OP_INDEX + hash;
            } 
            else {
                this->seenPixels[hash] = currPixel;

                // diff
                std::vector<int> diff = currPixel - prevPixel;
                if (diffValid(diff)) {
                    byte byteAcc = QOI_OP_DIFF;
                    int currShift = 4;
                    int bias = 2;
                    for (unsigned int i = 0; i < diff.size(); i++) {
                        // in range of [0,3] ie. two bits
                        int biasedDiff = (diff[i] + bias);
                        int biasedDiffShift = (biasedDiff << currShift);

                        byteAcc += biasedDiffShift;
                        
                        currShift = (currShift - 2);

                        if (currShift == -2) {
                            byteArray[byteArrayIdx++] = byteAcc;
                            byteAcc = 0;
                            currShift = 6;
                        }
                    }
                    if (currShift != 6) {byteArrayIdx++;}
                } 
                else {
                    byteArray[byteArrayIdx++] = QOI_OP_CHANNELS;
                    writePixel(currPixel, byteArray, byteArrayIdx);
                }
            }
        }
        
        prevPixel = currPixel;
    }
    // end marker
    if (line == bufferPtr->rows - 1) {
        int zeros = 7;
        while (zeros-- > 0) {
            byteArray[byteArrayIdx++] = 0;
        }
        byteArray[byteArrayIdx++] = 1;
        printf("size of the compressed data in bytes: %d\n", byteArrayIdx);
    }
}

// DECODERS

void QOIdecoder(byte* input, byte* output) {
    int readIdx = 0;
    int writeIdx = 0;

    // "qoif"
    if (int32fromBytes(input, readIdx) != 0x716f6966) {
        // false
    }
    int cols = int32fromBytes(input, readIdx);
    int rows = int32fromBytes(input, readIdx);
    int channels = (int) input[readIdx++];
    // color space
    readIdx++;

    int outputSize = channels * rows * cols;

    std::vector<Pixel> seenPixels(64, Pixel(channels));
    Pixel prevPixel(channels);

    while (writeIdx < outputSize) {
        if (input[readIdx] == QOI_OP_CHANNELS) {
            readIdx++;
            Pixel newPixel(channels, readIdx, input);
            prevPixel = newPixel;
            writePixel(prevPixel, output, writeIdx);

            readIdx += channels;

            byte hash = hashFunction(newPixel);
            seenPixels[hash] = newPixel;
        } else if ((input[readIdx] & QOI_1ST_2BIT_MASK) == QOI_OP_RUN) {
            byte runLength = (input[readIdx] & QOI_LAST_6BIT_MASK) + 1;
            
            for (byte i = 0; i < runLength; i++) {
                writePixel(prevPixel, output, writeIdx);
            }
            readIdx++;
        } else if ((input[readIdx] & QOI_1ST_2BIT_MASK) == QOI_OP_INDEX) {
            byte index = (input[readIdx] & QOI_LAST_6BIT_MASK);
            writePixel(seenPixels[index], output, writeIdx);
            readIdx++;
        } else if ((input[readIdx] & QOI_1ST_2BIT_MASK) == QOI_OP_DIFF) {
            int bitIdx = 2;
            std::vector<byte> data = prevPixel.Data();
            for (int i = 0; i < channels; i++) {
                int diff;
                switch (bitIdx)
                {
                    case 0:
                        diff = (int) (input[readIdx] & QOI_1ST_2BIT_MASK) >> 6;
                        break;
                    case 2:
                        diff = (int) (input[readIdx] & QOI_2ND_2BIT_MASK) >> 4;
                        break;
                    case 4:
                        diff = (int) (input[readIdx] & QOI_3RD_2BIT_MASK) >> 2;
                        break;
                    default:
                        diff = (int) (input[readIdx] & QOI_4TH_2BIT_MASK);
                        readIdx++;
                        break;
                }
                bitIdx = (bitIdx + 2) % 8;
                // apply bias
                diff -= 2;
                data[i] = (byte) ((((int) data[i]) + diff) & 0xff);
            }
            Pixel newPixel(data);
            prevPixel = newPixel;
            writePixel(newPixel, output, writeIdx);
            if (bitIdx != 0) {readIdx++;}

        }
        // end marker (not necessarry due to loop condition on write)
    }
}

// HELPERS

byte hashFunction(Pixel& pixel) {
    int sum = 0;
    for (unsigned int i = 0; i < pixel.Data().size(); i++) {
        sum += (int) pixel.Data()[i];
    }
    return sum % 64;
}

void int32toBytes(int int32, byte* byteArray, int& byteArrayIdx) {
    byteArray[byteArrayIdx++] = (int32 >> 24) & 0xFF;
    byteArray[byteArrayIdx++] = (int32 >> 16) & 0xFF;
    byteArray[byteArrayIdx++] = (int32 >> 8) & 0xFF;
    byteArray[byteArrayIdx++] = int32 & 0xFF;
}

int int32fromBytes(byte* byteArray, int& byteArrayIdx) {
    int int32 = (byteArray[byteArrayIdx]   << 24) +
                (byteArray[byteArrayIdx+1] << 16) +
                (byteArray[byteArrayIdx+2] << 8)  +
                (byteArray[byteArrayIdx+3]);
    byteArrayIdx += 4;
    return int32;
}

bool diffValid(std::vector<int> diff) {
    for (unsigned int i = 0; i < diff.size(); i++) {
        if (-2 > diff[i] || diff[i] > 1) {
            return false;
        }
    }
    return true;
}

void writePixel(Pixel pixel, byte* output, int& writeIdx) {
    for (unsigned int i = 0; i < pixel.Data().size(); i++) {
        output[writeIdx++] = pixel.Data()[i];
    }
}
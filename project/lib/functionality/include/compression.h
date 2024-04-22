#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdint.h>
#include "Buffer.h"

typedef uint8_t byte;

class Pixel {
    private:
        std::vector<byte> data;
    public:
        // default constructor
        Pixel(int channels);
        Pixel(std::vector<byte> data);
        Pixel(int channels, int index,  byte* memory);

        std::vector<byte>& Data();

        bool operator==(const Pixel& other) const;
        std::vector<int> operator-(const Pixel& other) const;
};

class Compression {
    public:
        Compression() {};
        virtual Buffer* InputBuffer() = 0;
        virtual void EncodeLine(int& line) = 0;
        virtual ~Compression() {};
};

const byte QOI_OP_RUN = 192; // 11000000
const byte QOI_OP_INDEX = 0; // 00000000
const byte QOI_OP_DIFF = 64; // 01000000
const byte QOI_OP_CHANNELS = 254; // 11111110

const byte QOI_1ST_2BIT_MASK = 192;
const byte QOI_2ND_2BIT_MASK = 48;
const byte QOI_3RD_2BIT_MASK = 12;
const byte QOI_4TH_2BIT_MASK = 3;

const byte QOI_LAST_6BIT_MASK = 63;

class QOI : public Compression {
    private:
        Buffer* bufferPtr;
        byte* output;
        int writeIdx;
        int lastPixelIdx;
        // 
        std::vector<Pixel> seenPixels;
        //
        Pixel prevPixel;
        //
        int run;

        void QOIheader();
    
    public:
        QOI(Buffer* bufferPtr, byte* output);
        Buffer* InputBuffer();
        void EncodeLine(int& line);
        void Decode();
        ~QOI() {};
};

// DECODERS

void QOIdecoder(byte* input, byte* output);

// HELPERS

byte hashFunction(Pixel& pixel);

void int32toBytes(int int32, byte* output, int& writeIdx);

int int32fromBytes(byte* input, int& readIdx);

bool diffValid(std::vector<int> diff);

void writePixel(Pixel pixel, byte* output, int& writeIdx);

#endif // COMPRESSION_H
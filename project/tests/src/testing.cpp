#include "testing.h"

cv::Mat byteArrayToImg(byte* byteArray, int channels, int rows, int cols) {
    cv::Mat image(rows, cols, channels == 1 ? CV_8UC1 : CV_8UC3);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (channels == 1) {
                uchar intensity = byteArray[i * cols + j];
                image.at<uchar>(i, j) = intensity;
            } else {
                for (int c = 0; c < channels; c++) {
                    int idx = (i * cols + j) * channels + c;
                    image.at<cv::Vec3b>(i, j)[c] = byteArray[idx];
                }
            }
        }
    }
    return image;
}

cv::Mat uint16ArrayToImg(uint16_t* uint16Array, int channels, int rows, int cols) {
    cv::Mat image(rows, cols, channels == 1 ? CV_8UC1 : CV_8UC3);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (channels == 1) {
                uint16_t intensity = uint16Array[i * cols + j];
                if (intensity > 255) {intensity = 255;}
                image.at<uchar>(i, j) = (uchar) intensity;
            } else {
                for (int c = 0; c < channels; c++) {
                    int idx = (i * cols + j) * channels + c;
                    image.at<cv::Vec3b>(i, j)[c] = uint16Array[idx];
                }
            }
        }
    }
    return image;
}

void checkCorrectness(cv::Mat& y) {
    // Display the predicate image
    cv::imshow("predicate image", y);
    cv::waitKey(0);
}


CameraHS makeCameraHS() {
    int channels = 3;
    int rows = 360;
    int cols = 640;

    bool snapshot = true;

    std::string devicePath = "/dev/video0";
    CameraHS cameraHS(devicePath, snapshot, channels, rows, cols);

    return cameraHS;

}

CameraSim makeCameraSim(bool snapshot) {
    int channels = 3;
    int rows = 576;
    int cols = 768;

    CameraSim cameraSim(snapshot, channels, rows, cols);
    std::string path = "qualititative/qualititative.png";
    cv::Mat inputImage = cv::imread(path);
    std::vector<byte> vectorInputImage;
    for (int i = 0; i < inputImage.rows; i++) {
        for (int j = 0; j < inputImage.cols; j++) {
            cv::Vec3b pixel = inputImage.at<cv::Vec3b>(i,j);
            for (int c = 0; c < inputImage.channels(); c++) {
                vectorInputImage.push_back((byte) pixel[c]);
            }
        }
    }
    cameraSim.StoreData(vectorInputImage);

    return cameraSim;
}

void test(bool hasDevice, bool snapshot, 
          std::string functionType, std::string compressionType) 
{
    Host host;
    if (hasDevice) {
        printf("Hyperspectral camera snapshot is %d\n", 1);
        CameraHS cam = makeCameraHS();
        host.Configure(&cam);
        function(host, functionType, compressionType);
    } else {
        printf("Simulation camera snapshot is %d\n", snapshot);
        CameraSim cam = makeCameraSim(snapshot);
        host.Configure(&cam);
        function(host, functionType, compressionType);
    }
}

void function(Host& host, 
              std::string functionType, 
              std::string compressionType) 
{
    int channels = host.camSensor->Channels();
    int cols = host.camSensor->Cols();
    int rows = host.camSensor->Rows();

    int outChannels;
    int outRows;
    int outCols;

    cv::Mat hostOutput;
    byte* output;
    uint16_t* output_uint16;
    
    if (functionType == "sobel") {
        outChannels = channels;
        outRows = rows;
        outCols = cols;
        
        if (compressionType == "QOI") {
            output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));

            int headerSize = 14;
            int endMarkerSize = 8;
            int maxSize = (outRows * 
                           outCols * 
                           (outChannels + 1) + 
                           headerSize + 
                           endMarkerSize);

            byte* outputEncoding = (byte*) malloc(maxSize*sizeof(byte));

            host.Sobel(outputEncoding, compressionType);
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);

            hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
            free(output);
        } else {
            output_uint16 = (uint16_t*) malloc((outChannels*
                                                 outRows*
                                                 outCols*
                                                 sizeof(uint16_t)));
            
            host.Sobel(output_uint16);

            hostOutput = uint16ArrayToImg(output_uint16, outChannels, outRows, outCols);
            free(output_uint16);
        }
    } 
    else if (functionType == "gaussianBlur") {
        outChannels = channels;
        outRows = rows;
        outCols = cols;

        output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));

        int kernelHeight = 3; 
        int kernelWidth = 3;
        double sigmaX = 1;
        double sigmaY = 0;
        
        if (compressionType == "QOI") {
            int headerSize = 14;
            int endMarkerSize = 8;
            int maxSize = (outRows * 
                           outCols * 
                           (outChannels + 1) + 
                           headerSize + 
                           endMarkerSize);

            byte* outputEncoding = (byte*) malloc(maxSize*sizeof(byte));

            host.GaussianBlur(outputEncoding, kernelHeight, kernelWidth, sigmaX, sigmaY, 
                              compressionType);
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);
        } else {
            host.GaussianBlur(output, kernelHeight, kernelWidth, sigmaX, sigmaY);
        }
        hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
        free(output);
    }
    else if (functionType == "cannyEdge") {
        outChannels = 1;
        outRows = rows;
        outCols = cols;

        output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));

        uint16_t lowThreshold = 230;
        uint16_t highThreshold = 240;
        
        if (compressionType == "QOI") {
            int headerSize = 14;
            int endMarkerSize = 8;
            int maxSize = (outRows * 
                           outCols * 
                           (outChannels + 1) + 
                           headerSize + 
                           endMarkerSize);

            byte* outputEncoding = (byte*) malloc(maxSize*sizeof(byte));

            host.CannyEdge(outputEncoding, lowThreshold, highThreshold,
                           compressionType);
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);
        } else {
            host.CannyEdge(output, lowThreshold, highThreshold);
        }
        hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
        free(output);
    } else {
        outChannels = channels;
        outRows = rows;
        outCols = cols;

        output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));
        
        if (compressionType == "QOI") {
            int headerSize = 14;
            int endMarkerSize = 8;
            int maxSize = (outRows * 
                           outCols * 
                           (outChannels + 1) + 
                           headerSize + 
                           endMarkerSize);

            byte* outputEncoding = (byte*) malloc(maxSize*sizeof(byte));

            host.QOIencode(outputEncoding);
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);
        }
        hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
        free(output);
    }

    printf("c %d r %d co %d\n", hostOutput.channels(), hostOutput.rows, hostOutput.cols);
    checkCorrectness(hostOutput);
}
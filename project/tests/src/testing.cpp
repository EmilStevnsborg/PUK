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
    std::string path = "cases/Background_Subtraction_Tutorial_frame_1.png";
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

void test(std::string functionType, bool hasDevice, bool snapshot) {
    Host host;
    if (hasDevice) {
        printf("Hyperspectral camera snapshot is %d\n", 1);
        CameraHS cam = makeCameraHS();
        host.Configure(&cam);
        function(functionType, host);
    } else {
        printf("Simulation camera snapshot is %d\n", snapshot);
        CameraSim cam = makeCameraSim(snapshot);
        host.Configure(&cam);
        function(functionType, host);
    }
}

void function(std::string functionType, Host& host) {
    int channels = host.camSensor->Channels();
    int cols = host.camSensor->Cols();
    int rows = host.camSensor->Rows();

    int outChannels;
    int outRows;
    int outCols;

    cv::Mat hostOutput;
    byte* output;
    
    if (functionType == "sobel") {
        outChannels = channels;
        outRows = rows;
        outCols = cols;

        output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));

        host.Sobel(output);
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
        
        host.GaussianBlur(output, kernelHeight, kernelWidth, sigmaX, sigmaY);
    }
    else if (functionType == "cannyEdge") {
        outChannels = 1;
        outRows = rows;
        outCols = cols;

        output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));

        uint16_t lowThreshold = 20;
        uint16_t highThreshold = 70;
        
        host.CannyEdge(output, lowThreshold, highThreshold);
    } else if (functionType == "QOIencode") {
        outChannels = channels;
        outRows = rows;
        outCols = cols;
        
        int headerSize = 14;
        int endMarkerSize = 8;
        int maxSize = outRows * outCols * (outChannels + 1) + headerSize + endMarkerSize;

        byte* outputEncoding = (byte*) malloc(maxSize*sizeof(byte));
        host.Encode(outputEncoding, "QOI", "");

        output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));
        QOIdecoder(outputEncoding, output);

        free(outputEncoding);
    } else {
    }

    hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
    checkCorrectness(hostOutput);

    free(output);
}
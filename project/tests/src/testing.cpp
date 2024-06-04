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

    std::string devicePath = "/dev/video2";
    CameraHS cameraHS(devicePath, snapshot, channels, rows, cols);

    return cameraHS;

}

CameraSim makeCameraSim(bool snapshot) {
    int channels = 3;
    int rows = 360;
    int cols = 640;

    CameraSim cameraSim(snapshot, channels, rows, cols);
    std::string path = "test.png";
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

void performanceTest(std::string functionType, std::string compressionType) {
    CameraSim cam = makeCameraSim(false);
    int channels = cam.Channels();
    int cols = cam.Cols();
    int rows = cam.Rows();

    Host host;
    host.Configure(&cam);

    int warmup = 300;
    int iterations = 400;
    float time_e = 0;
    float time_ocv = 0;

    for (int i = 0; i < iterations; i++) {
        std::chrono::high_resolution_clock::time_point start_e;
        std::chrono::high_resolution_clock::time_point stop_e;

        std::chrono::high_resolution_clock::time_point start_ocv;
        std::chrono::high_resolution_clock::time_point stop_ocv;

        cv::Mat output_ocv;

        if (functionType == "gaussianBlur") {
            int kernelHeight = 3; 
            int kernelWidth = 3;
            double sigmaX = 1;
            double sigmaY = 0;

            // embedded

            if (compressionType == "QOI") {
                byte* outputEncoding = (byte*) malloc(channels*rows*cols*sizeof(byte));

                start_e = std::chrono::high_resolution_clock::now();
                host.GaussianBlur(outputEncoding, kernelHeight, kernelWidth, sigmaX, sigmaY, 
                                compressionType);
                stop_e = std::chrono::high_resolution_clock::now();
            } else {
                byte* output = (byte*) malloc(channels*rows*cols*sizeof(byte));
                start_e = std::chrono::high_resolution_clock::now();
                host.GaussianBlur(output, kernelHeight, kernelWidth, sigmaX, sigmaY, "");
                stop_e = std::chrono::high_resolution_clock::now();
                free(output);
            }
            
            // opencv
            start_ocv = std::chrono::high_resolution_clock::now();
            byte* output = (byte*) malloc(channels*rows*cols*sizeof(byte));
            Buffer buffer(channels, rows, cols, rows, false, output);
            cam.Stream(&buffer);
            cv::Mat img = byteArrayToImg(output, channels, rows, cols);
            cv::GaussianBlur(img, output_ocv, cv::Size(3,3), 1);
            stop_ocv = std::chrono::high_resolution_clock::now();
        }
        if (functionType == "medianBlur") {
            int kernelHeight = 3; 
            int kernelWidth = 3;

            // embedded
            if (compressionType == "QOI") {
                byte* outputEncoding = (byte*) malloc(channels*rows*cols*sizeof(byte));

                start_e = std::chrono::high_resolution_clock::now();
                host.MedianBlur(outputEncoding, kernelHeight, kernelWidth, compressionType);
                stop_e = std::chrono::high_resolution_clock::now();
            } else {
                uint16_t* output_uint16 = (uint16_t*) malloc((channels*
                                                            rows*
                                                            cols*
                                                            sizeof(uint16_t)));
                start_e = std::chrono::high_resolution_clock::now();
                host.MedianBlur(output_uint16, kernelHeight, kernelWidth);
                stop_e = std::chrono::high_resolution_clock::now();
                free(output_uint16);
            }

            // opencv
            start_ocv = std::chrono::high_resolution_clock::now();
            byte* output = (byte*) malloc(channels*rows*cols*sizeof(byte));
            Buffer buffer(channels, rows, cols, rows, false, output);
            cam.Stream(&buffer);
            cv::Mat img = byteArrayToImg(output, channels, rows, cols);
            cv::medianBlur(img, output_ocv, 3);
            stop_ocv = std::chrono::high_resolution_clock::now();
        }
        if (functionType == "sobel") {
            // embedded

            if (compressionType == "QOI") {
                byte* outputEncoding = (byte*) malloc(channels*rows*cols*sizeof(byte));

                start_e = std::chrono::high_resolution_clock::now();
                host.Sobel(outputEncoding, compressionType);
                stop_e = std::chrono::high_resolution_clock::now();
            } else {
                uint16_t* output_uint16 = (uint16_t*) malloc((channels*
                                                            rows*
                                                            cols*
                                                            sizeof(uint16_t)));
                start_e = std::chrono::high_resolution_clock::now();        
                host.Sobel(output_uint16);
                stop_e = std::chrono::high_resolution_clock::now();
                free(output_uint16);
            }

            // opencv
            start_ocv = std::chrono::high_resolution_clock::now();
            std::vector<cv::Mat> colors(3);
            byte* output = (byte*) malloc(channels*rows*cols*sizeof(byte));
            Buffer buffer(channels, rows, cols, rows, false, output);
            cam.Stream(&buffer);
            cv::Mat img = byteArrayToImg(output, channels, rows, cols);
            cv::split(img, colors);

            for (auto& c : colors) {  
                cv::Mat gx, gy;
                cv::Sobel(c, gx, CV_32F, 1, 0, 3, 1.0, 0.0, cv::BORDER_REFLECT_101);
                cv::Sobel(c, gy, CV_32F, 0, 1, 3, 1.0, 0.0, cv::BORDER_REFLECT_101);
                // compute gradient magnitude
                cv::Mat magnitude;
                cv::magnitude(gx, gy, magnitude);
            }
            stop_ocv = std::chrono::high_resolution_clock::now();
        }
        if (functionType == "cannyEdge") {
            uint16_t lowThreshold = 40;
            uint16_t highThreshold = 60;
        
            // embedded
            if (compressionType == "QOI") {

                byte* outputEncoding = (byte*) malloc((1+1)*rows*cols*sizeof(byte));

                start_e = std::chrono::high_resolution_clock::now();
                host.CannyEdge(outputEncoding, lowThreshold, highThreshold,
                            compressionType);
                stop_e = std::chrono::high_resolution_clock::now();
            } else {
                byte* output = (byte*) malloc(1*rows*cols*sizeof(byte));
                start_e = std::chrono::high_resolution_clock::now();
                host.CannyEdge(output, lowThreshold, highThreshold);
                stop_e = std::chrono::high_resolution_clock::now();
                free(output);
            }

            // opencv
            start_ocv = std::chrono::high_resolution_clock::now();
            cv::Mat gray;
            cv::Mat edges;
            byte* output = (byte*) malloc(channels*rows*cols*sizeof(byte));
            Buffer buffer(channels, rows, cols, rows, false, output);
            cam.Stream(&buffer);
            cv::Mat img = byteArrayToImg(output, channels, rows, cols);
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(img, edges, 40, 60); 
            stop_ocv = std::chrono::high_resolution_clock::now();
        }
        if (functionType == "") {
            start_ocv = std::chrono::high_resolution_clock::now();
            stop_ocv = std::chrono::high_resolution_clock::now();

            byte* outputEncoding = (byte*) malloc(channels*rows*cols*sizeof(byte));
            start_e = std::chrono::high_resolution_clock::now();
            host.QOIencode(outputEncoding);
            stop_e = std::chrono::high_resolution_clock::now();
        }
        auto duration_e = std::chrono::duration_cast<std::chrono::microseconds>(stop_e - start_e);
        auto duration_ocv = std::chrono::duration_cast<std::chrono::microseconds>(stop_ocv - start_ocv);

        if (i >= warmup) {
            time_ocv += duration_ocv.count();
            time_e += duration_e.count();
        }
    }
    printf("function %s duration_e %fmus duration_ocv %fmus\n", 
        functionType.c_str(), time_e/(iterations-warmup), time_ocv/(iterations-warmup));
}

void memoryTestEmbedded(std::string functionType, std::string compressionType) {
    CameraHS cam = makeCameraHS();
    int channels = cam.Channels();
    int cols = cam.Cols();
    int rows = cam.Rows();

    Host host;
    host.Configure(&cam);

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point stop;

    if (functionType == "gaussianBlur") {

        int kernelHeight = 3; 
        int kernelWidth = 3;
        double sigmaX = 1;
        double sigmaY = 0;

        if (compressionType == "QOI") {
            byte* outputEncoding = (byte*) malloc(channels*rows*cols*sizeof(byte));

            start = std::chrono::high_resolution_clock::now();
            host.GaussianBlur(outputEncoding, kernelHeight, kernelWidth, sigmaX, sigmaY, 
                            compressionType);
            stop = std::chrono::high_resolution_clock::now();
            free(outputEncoding);
        } else {
            byte* output = (byte*) malloc(channels*rows*cols*sizeof(byte));
            start = std::chrono::high_resolution_clock::now();
            host.GaussianBlur(output, kernelHeight, kernelWidth, sigmaX, sigmaY, "");
            stop = std::chrono::high_resolution_clock::now();
            free(output);
        }
    }
    if (functionType == "medianBlur") {

        int kernelHeight = 3; 
        int kernelWidth = 3;
        
        if (compressionType == "QOI") {
            byte* outputEncoding = (byte*) malloc(channels*rows*cols*sizeof(byte));

            start = std::chrono::high_resolution_clock::now();
            host.MedianBlur(outputEncoding, kernelHeight, kernelWidth, compressionType);
            stop = std::chrono::high_resolution_clock::now();
            free(outputEncoding);
        } else {
            uint16_t* output_uint16 = (uint16_t*) malloc((channels*
                                                        rows*
                                                        cols*
                                                        sizeof(uint16_t)));
            start = std::chrono::high_resolution_clock::now();
            host.MedianBlur(output_uint16, kernelHeight, kernelWidth);
            stop = std::chrono::high_resolution_clock::now();
            free(output_uint16);
        }
    }
    if (functionType == "sobel") {
        
        if (compressionType == "QOI") {
            byte* outputEncoding = (byte*) malloc(channels*rows*cols*sizeof(byte));

            start = std::chrono::high_resolution_clock::now();
            host.Sobel(outputEncoding, compressionType);
            stop = std::chrono::high_resolution_clock::now();
            free(outputEncoding);
        } else {
            uint16_t* output_uint16 = (uint16_t*) malloc((channels*
                                                        rows*
                                                        cols*
                                                        sizeof(uint16_t)));
            start = std::chrono::high_resolution_clock::now();        
            host.Sobel(output_uint16);
            stop = std::chrono::high_resolution_clock::now();
            free(output_uint16);
        }

    }
    if (functionType == "cannyEdge") {

        uint16_t lowThreshold = 40;
        uint16_t highThreshold = 60;
        
        if (compressionType == "QOI") {

            byte* outputEncoding = (byte*) malloc((1+1)*rows*cols*sizeof(byte));

            start = std::chrono::high_resolution_clock::now();
            host.CannyEdge(outputEncoding, lowThreshold, highThreshold,
                        compressionType);
            stop = std::chrono::high_resolution_clock::now();
            free(outputEncoding);
        } else {
            byte* output = (byte*) malloc(1*rows*cols*sizeof(byte));
            start = std::chrono::high_resolution_clock::now();
            host.CannyEdge(output, lowThreshold, highThreshold);
            stop = std::chrono::high_resolution_clock::now();
            free(output);
        }
    }
    if (functionType == "" && compressionType == "QOI") {
        int headerSize = 14;
        int endMarkerSize = 8;
        int maxSize = (rows * 
                       cols * 
                       (channels + 1) + 
                       headerSize + 
                       endMarkerSize);

        byte* outputEncoding = (byte*) malloc(maxSize*sizeof(byte));

        start = std::chrono::high_resolution_clock::now();
        host.QOIencode(outputEncoding);
        stop = std::chrono::high_resolution_clock::now();
        free(outputEncoding);
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    printf("function %s duration %ldms\n", 
           functionType.c_str(), duration.count());
}

void memoryTestOCV(std::string functionType) {
    cv::VideoCapture cam;
    cv::Mat img;

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point stop;

    cam.open(2);
    cam >> img;
    cv::Mat output;
    if (functionType == "gaussianBlur") {
        start = std::chrono::high_resolution_clock::now();
        cv::GaussianBlur(img, output, cv::Size(3,3), 1);
        stop = std::chrono::high_resolution_clock::now();
    }
    if (functionType == "medianBlur") {
        start = std::chrono::high_resolution_clock::now();
        cv::medianBlur(img, output, 3);
        stop = std::chrono::high_resolution_clock::now();
    }
    if (functionType == "sobel") {
        start = std::chrono::high_resolution_clock::now();
        std::vector<cv::Mat> channels(3);
        cv::split(img, channels);

        for (auto& c : channels) {  
            cv::Mat gx, gy;
            cv::Sobel(c, gx, CV_32F, 1, 0, 3, 1.0, 0.0, cv::BORDER_REFLECT_101);
            cv::Sobel(c, gy, CV_32F, 0, 1, 3, 1.0, 0.0, cv::BORDER_REFLECT_101);
            // compute gradient magnitude
            cv::Mat magnitude;
            cv::magnitude(gx, gy, magnitude);
        }
        stop = std::chrono::high_resolution_clock::now();

    }
    if (functionType == "cannyEdge") {
        start = std::chrono::high_resolution_clock::now();
        cv::Mat gray;
        cv::Mat edges;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        cv::Canny(img, edges, 40, 60); 
        stop = std::chrono::high_resolution_clock::now();
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    printf("function %s duration %ldms\n", 
           functionType.c_str(), duration.count());
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

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point stop;
    
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

            start = std::chrono::high_resolution_clock::now();
            host.Sobel(outputEncoding, compressionType);
            stop = std::chrono::high_resolution_clock::now();
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);

            hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
            free(output);
        } else {
            output_uint16 = (uint16_t*) malloc((outChannels*
                                                 outRows*
                                                 outCols*
                                                 sizeof(uint16_t)));
            
            start = std::chrono::high_resolution_clock::now();
            host.Sobel(output_uint16);
            stop = std::chrono::high_resolution_clock::now();

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

            start = std::chrono::high_resolution_clock::now();
            host.GaussianBlur(outputEncoding, kernelHeight, kernelWidth, sigmaX, sigmaY, 
                              compressionType);
            stop = std::chrono::high_resolution_clock::now();
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);
        } else {
            start = std::chrono::high_resolution_clock::now();
            host.GaussianBlur(output, kernelHeight, kernelWidth, sigmaX, sigmaY);
            stop = std::chrono::high_resolution_clock::now();
        }
        hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
        free(output);
    }
    else if (functionType == "cannyEdge") {
        outChannels = 1;
        outRows = rows;
        outCols = cols;

        output = (byte*) malloc(outChannels*outRows*outCols*sizeof(byte));

        uint16_t lowThreshold = 40;
        uint16_t highThreshold = 60;
        
        if (compressionType == "QOI") {
            int headerSize = 14;
            int endMarkerSize = 8;
            int maxSize = (outRows * 
                           outCols * 
                           (outChannels + 1) + 
                           headerSize + 
                           endMarkerSize);

            byte* outputEncoding = (byte*) malloc(maxSize*sizeof(byte));

            start = std::chrono::high_resolution_clock::now();
            host.CannyEdge(outputEncoding, lowThreshold, highThreshold,
                           compressionType);
            stop = std::chrono::high_resolution_clock::now();
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);
        } else {
            start = std::chrono::high_resolution_clock::now();
            host.CannyEdge(output, lowThreshold, highThreshold);
            stop = std::chrono::high_resolution_clock::now();
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

            start = std::chrono::high_resolution_clock::now();
            host.QOIencode(outputEncoding);
            stop = std::chrono::high_resolution_clock::now();
            
            QOIdecoder(outputEncoding, output);
            free(outputEncoding);
        }
        hostOutput = byteArrayToImg(output, outChannels, outRows, outCols);
        free(output);
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    printf("function %s duration %ldms c %d r %d co %d\n", 
           functionType.c_str(), duration.count(),
           hostOutput.channels(), hostOutput.rows, hostOutput.cols);
    checkCorrectness(hostOutput);
}
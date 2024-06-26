#include "Host.h"
#include "CameraSim.h"
#include "CameraHS.h"
#include "util.h"
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <string>
#include <fstream>

namespace py = pybind11;

typedef uint8_t byte;

void copyData(Buffer* outputBuffer, py::array_t<uint8_t>& array) {

    // Copy data from the outputBuffer to the NumPy array
    for (int i = 0; i < outputBuffer->rows; i++) {
        for (int j = 0; j < outputBuffer->cols; j++) {
            for (int c = 0; c < outputBuffer->channels; c++) {
                int idx = i*outputBuffer->lineSize + j*outputBuffer->channels + c;
                
                array.mutable_data()[idx] = outputBuffer->Memory<byte>()[idx];
            }
        }
    }
}

PYBIND11_MODULE(EmbeddedCV, m) {
    py::class_<CameraHS>(m, "CameraHS")
        .def(py::init<std::string, bool, int, int, int>());
    
    py::class_<CameraSim>(m, "CameraSim")
        .def(py::init<bool, int, int, int>())
        .def("StoreData", [](CameraSim &self, py::array_t<uint8_t> array) {
            std::vector<uint8_t> data(array.size());
            std::memcpy(data.data(), array.data(), array.size() * sizeof(uint8_t));
            self.StoreData(data);
        })
        .def("GetImage", [](CameraSim &self) {
            auto image = self.GetImage();
            py::array_t<uint8_t> numpy_array(image.size());
            std::memcpy(numpy_array.mutable_data(), image.data(), image.size() * sizeof(uint8_t));

            return numpy_array;
        });

    py::class_<Host>(m, "Host")
        .def(py::init<>())
        .def("Configure", [] (Host& self, CameraHS* cam) {
            self.Configure(cam);
        })
        .def("Configure", [] (Host& self, CameraSim* cam) {
            self.Configure(cam);
        })
        .def("CamSensor", [] (Host& self) {
            return self.camSensor;
        })
        .def("Sanity", [] (Host& self, py::array_t<uint8_t>& array) {
            // Allocate memory for the output buffer
            Buffer outputBuffer(self.channels, self.rows, self.cols, self.rows, false, true);

            // stream entire image (snapshot)
            self.camSensor->Stream(&outputBuffer);
            
            // Copy data from the outputBuffer to the NumPy array
            copyData(&outputBuffer, array);

            // Free the memory of the output buffer
            outputBuffer.FreeMemory();

        })
        .def("MedianBlur", [] (Host& self, py::array_t<uint8_t>& array, 
                               int kernelHeight, int kernelWidth,
                               std::string compressionType) {

            self.MedianBlur(array.mutable_data(), 
                            kernelHeight, kernelWidth,
                            compressionType);

        })
        .def("MedianBlur", [] (Host& self, py::array_t<uint16_t>& array, 
                               int kernelHeight, int kernelWidth) {

            self.MedianBlur(array.mutable_data(), 
                            kernelHeight, kernelWidth);

        })
        .def("GaussianBlur", [] (Host& self, py::array_t<uint8_t>& array, 
                                 int kernelHeight, int kernelWidth,
                                 double sigmaX, double sigmaY,
                                 std::string compressionType) {

            self.GaussianBlur(array.mutable_data(), 
                              kernelHeight, kernelWidth, 
                              sigmaX, sigmaY,
                              compressionType);

        })
        .def("GaussianBlur", [] (Host& self, py::array_t<uint8_t>& array, 
                                 int kernelHeight, int kernelWidth,
                                 double sigmaX, double sigmaY) {

            self.GaussianBlur(array.mutable_data(), 
                              kernelHeight, kernelWidth, 
                              sigmaX, sigmaY);

        })
        .def("Sobel", [] (Host& self, py::array_t<uint8_t>& array,
                          std::string compressionType) {

            self.Sobel(array.mutable_data(), compressionType);

        })
        .def("Sobel", [] (Host& self, py::array_t<uint16_t>& array) {

            self.Sobel(array.mutable_data());

        })
        .def("CannyEdge", [] (Host& self, py::array_t<uint8_t>& array, 
                              byte lowThreshold, byte highThreshold,
                              std::string compressionType) {

            self.CannyEdge(array.mutable_data(), 
                           lowThreshold, highThreshold,
                           compressionType);
            
        })
        .def("CannyEdge", [] (Host& self, py::array_t<uint8_t>& array, 
                              byte lowThreshold, byte highThreshold) {

            self.CannyEdge(array.mutable_data(), 
                           lowThreshold, highThreshold);
            
        })
        .def("QOIencode", [] (Host& self, py::array_t<uint8_t>& array) {

            // Call the GaussianBlur method with the output buffer
            self.QOIencode(array.mutable_data());
            
        })
        .def("QOIdecode", [] (py::array_t<uint8_t>& input, 
                              py::array_t<uint8_t>& output) {

            // Call the GaussianBlur method with the output buffer
            QOIdecoder(input.mutable_data(), output.mutable_data());
            
        });
}


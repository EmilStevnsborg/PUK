#include "Host.h"
#include "CamSimulator.h"
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
    py::class_<CamSimulator>(m, "CamSimulator")
        .def(py::init<int, int, int>())
        .def("StoreData", [](CamSimulator &self, py::array_t<uint8_t> array) {
            std::vector<uint8_t> data(array.size());
            std::memcpy(data.data(), array.data(), array.size() * sizeof(uint8_t));
            self.StoreData(data);
        })
        .def("GetImage", [](CamSimulator &self) {
            auto image = self.GetImage();
            py::array_t<uint8_t> numpy_array(image.size());
            std::memcpy(numpy_array.mutable_data(), image.data(), image.size() * sizeof(uint8_t));

            return numpy_array;
        });

    py::class_<Host>(m, "Host")
        .def(py::init<CamSimulator*, int, int, int>())
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
                           int kernelHeight, int kernelWidth) {
            // Allocate memory for the output buffer
            Buffer outputBuffer(self.channels, self.rows, self.cols, self.rows, false, true);

            // Call the GaussianBlur method with the output buffer
            self.MedianBlur(&outputBuffer, kernelHeight, kernelWidth);
            
            // Copy data from the outputBuffer to the NumPy array
            copyData(&outputBuffer, array);

            // Free the memory of the output buffer
            outputBuffer.FreeMemory();
        })
        .def("GaussianBlur", [] (Host& self, py::array_t<uint8_t>& array, 
                                int kernelHeight, int kernelWidth,
                                double sigmaX, double sigmaY) {
            // Allocate memory for the output buffer
            Buffer outputBuffer(self.channels, self.rows, self.cols, self.rows, false, true);

            // Call the GaussianBlur method with the output buffer
            self.GaussianBlur(&outputBuffer, kernelHeight, kernelWidth, sigmaX, sigmaY);
            
            // Copy data from the outputBuffer to the NumPy array
            copyData(&outputBuffer, array);

            // Free the memory of the output buffer
            outputBuffer.FreeMemory();
        })
        .def("Sobel", [] (Host& self, py::array_t<uint8_t>& array) {
            // Allocate memory for the output buffer
            Buffer outputBuffer(self.channels, self.rows, self.cols, self.rows, false, true);

            // Call the GaussianBlur method with the output buffer
            self.Sobel(&outputBuffer);

            // Copy data from the outputBuffer to the NumPy array
            copyData(&outputBuffer, array);
            
            // Free the memory of the output buffer
            outputBuffer.FreeMemory();
        })
        .def("CannyEdge", [] (Host& self, py::array_t<uint8_t>& array, 
                              byte lowThreshold, byte highThreshold) {
            // Allocate memory for the output buffer
            Buffer outputBuffer(1, self.rows, self.cols, self.rows, false, true);

            // Call the GaussianBlur method with the output buffer
            self.CannyEdge(&outputBuffer, lowThreshold, highThreshold);

            // Copy data from the outputBuffer to the NumPy array
            copyData(&outputBuffer, array);
            
            // Free the memory of the output buffer
            outputBuffer.FreeMemory();
        });
}


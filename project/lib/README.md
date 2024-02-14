# lib

This is a library consisting of functionality of simulating a camera `simulation/` and the functionality that should be applied to the incoming data `functionality/`
 
Camsimulator can store data captured by device or read from path as cv::Mat.
It also transforms this data into a byte (unsigned char) array when storing.

When streaming, it allocates new memory and copies the data into it.


# Simulation

Here one finds all the functionality that is related to simulating a camera.

In `src/` one finds all the source files (files of kind `.cpp`), and the headers to those source files in `include/`.

## imageReader

Functions used to read or snap images in the form of 

```cpp
cv::Mat
```

## simulators

Functions to store entire images in memory of form

```cpp
cv::Mat
```
Copies the data and stores in memory as a long byte array

### linescanSimulator

Includes function, which returns pointer for a designated line in memory

### snapshotSimulator

Includes function, which returns pointer to a beginning of byte array

## host

Calls simulator for data (whether that be a line or snapshot) and coordinates calls to functions in `../functionality/*/` such that correct data is received for computation.



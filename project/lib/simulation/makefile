# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g

OPENCV_CFLAGS := $(shell pkg-config --cflags opencv4)
OPENCV_LIBS := $(shell pkg-config --libs opencv4)

# Target and dependencies
TARGET := ImageReader/ImageReader
SOURCE := ImageReader/ImageReader.cpp
OBJECT := ImageReader/ImageReader.o

# Build rule
$(TARGET): $(OBJECT)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECT) $(OPENCV_LIBS)

$(OBJECT): $(SOURCE)
	$(CXX) $(CXXFLAGS) -c -o $(OBJECT) $(SOURCE) $(OPENCV_CFLAGS)

# Clean rule
clean:
	rm -f $(TARGET) $(OBJECT)

# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g

OPENCV_CFLAGS := $(shell pkg-config --cflags opencv4)
OPENCV_LIBS := $(shell pkg-config --libs opencv4)

# all headers from simulation
HEADERS := ../lib/simulation/include/

OBJECTS := ../lib/simulation/obj/*.o

# implicit -I.
# all headers current dir

all: 
	$(CXX) -o program $(CXXFLAGS) $(OPENCV_CFLAGS) -I$(HEADERS) program.cpp $(OBJECTS) $(OPENCV_LIBS)


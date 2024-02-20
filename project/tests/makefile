# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g

OPENCV_CFLAGS := $(shell pkg-config --cflags opencv4)
OPENCV_LIBS := $(shell pkg-config --libs opencv4)

# all headers from simulation
HEADERS := -I../lib/simulation/include/ -I../lib/functionality/include/

OBJECTS := ../lib/simulation/obj/*.o ../lib/functionality/obj/*.o 

# implicit -I.
# all headers current dir

all: 
	$(CXX) -o program $(CXXFLAGS) $(OPENCV_CFLAGS) $(HEADERS) program.cpp $(OBJECTS) $(OPENCV_LIBS)


# supress warnings from opemcv in terminal: export OPENCV_LOG_LEVEL=OFF

# check for memory leaks: valgrind --leak-check=yes ./program
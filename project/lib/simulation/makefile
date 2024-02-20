# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g

OPENCV_CFLAGS := $(shell pkg-config --cflags opencv4)
OPENCV_LIBS := $(shell pkg-config --libs opencv4)

LIBS := util CamSimulator

HEADERS := -I../functionality/include/ -Iinclude/

OBJECTS := $(foreach LIB, $(LIBS), obj/$(LIB).o)

all: $(OBJECTS)

# bygge object filer
# "$@ $<" (automatic variables OBJECT and SOURCE)
obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(OPENCV_CFLAGS)	$(OPENCV_LIBS) $(HEADERS)

# Clean rule
clean:
	rm -rf obj


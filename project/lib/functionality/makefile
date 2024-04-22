# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g -shared -fPIC

# CXXFLAGS += -O3

LIBS := CVfunctions compression Host Buffer \
		GaussianBlurLayer NonMaxSuppressionLayer \
		SobelLayer HysterisisLayer GrayScaleLayer \
		MinMaxNormLayer MedianBlurLayer CameraHS

OBJECTS := $(foreach LIB, $(LIBS), obj/$(LIB).o)

all: $(OBJECTS)

# bygge object filer
# "$@ $<" (automatic variables OBJECT and SOURCE)
obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c -o $@ $< -Iinclude/

# Clean rule
clean:
	rm -rf obj
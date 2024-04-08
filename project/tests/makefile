# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g

OPENCV_CFLAGS := $(shell pkg-config --cflags opencv4)
OPENCV_LIBS := $(shell pkg-config --libs opencv4)

LIBS := program testing

HEADERS_TEST := -Iinclude/

OBJECTS_TEST := $(foreach LIB, $(LIBS), obj/$(LIB).o)

# functionality and simulation

HEADERS_ := -I../lib/simulation/include/ -I../lib/functionality/include/

OBJECTS_ := ../lib/simulation/obj/*.o ../lib/functionality/obj/*.o 

# lib and local

HEADERS :=  $(HEADERS_) $(HEADERS_TEST)

OBJECTS := $(OBJECTS_) $(OBJECTS_TEST)

all: $(OBJECTS_TEST)
	$(CXX) -o program $(CXXFLAGS) $(OPENCV_CFLAGS) $(HEADERS) $(OBJECTS) $(OPENCV_LIBS)

# bygge object filer
# "$@ $<" (automatic variables OBJECT and SOURCE)
obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(OPENCV_CFLAGS)	$(OPENCV_LIBS) $(HEADERS)

# Clean rule
clean:
	rm -rf obj
# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -g -shared -fPIC -O3

LIBS := util CameraSim

HEADERS := -I../functionality/include/ -Iinclude/

OBJECTS := $(foreach LIB, $(LIBS), obj/$(LIB).o)

all: $(OBJECTS)

# bygge object filer
# "$@ $<" (automatic variables OBJECT and SOURCE)
obj/%.o: src/%.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(HEADERS)

# Clean rule
clean:
	rm -rf obj


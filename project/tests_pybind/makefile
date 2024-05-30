# Define constants and collections
PYTHON = python3.10
PYBIND_FLAGS = $(shell $(PYTHON) -m pybind11 --include)
CXXFLAGS += $(subst -I,-isystem ,$(PYBIND_FLAGS))
CXXFLAGS += -shared
PYBIND_SUFFIX = $(shell $(PYTHON)-config --extension-suffix)

# Generic C++ flags
CXXFLAGS += -std=c++17 -Wall -Wextra -Werror -pedantic -O3 -march=native -mtune=native -fopenmp -fPIC -O3


HEADERS := -I../lib/simulation/include/ -I../lib/functionality/include/ -L/usr/lib/x86_64-linux-gnu
OBJ := ../lib/simulation/obj/* ../lib/functionality/obj/*

ifeq ($(shell uname -s), Darwin)
	CXX = g++-13
	CXXFLAGS += -undefined dynamic_lookup
endif

all:
	$(CXX) $(CXXFLAGS) $(HEADERS) -o EmbeddedCV$(PYBIND_SUFFIX) EmbeddedCV.cpp $(OBJ)

clean:
	rm -f EmbeddedCV$(PYBIND_SUFFIX)
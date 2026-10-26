# C parameters
CXX = g++
CFLAGS = -Wall -O3

# Include the CPP wrapper makefile
CPP_WRAPPER_ROOT_DIR = ../../../
include ../../cpp_wrapper.mk

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

complex: $(CPP_WRAPPER_LIB)
	$(CXX) $(CFLAGS) complex_test.cpp -o complex_test $(LINKER_FLAGS)

clean:
	rm complex_test

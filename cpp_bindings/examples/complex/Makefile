# C parameters
CC = g++
CFLAGS = -Wall -O3

# Include the CPP wrapper makefile
CPP_WRAPPER_ROOT_DIR = ../../../
include ../../cpp_wrapper.mk

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

complex:
	$(CC) $(CFLAGS) complex_test.cpp $(CPP_WRAPPER_COMPLEX_SRC) -o complex_test $(LINKER_FLAGS)

clean:
	rm complex_test

# C parameters
CC = g++
CFLAGS = -Wall -O3

# Include the CPP wrapper makefile
CPP_WRAPPER_ROOT_DIR = ../../../
include ../../cpp_wrapper.mk

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

average:
	$(CC) $(CFLAGS) average_test.cpp $(CPP_WRAPPER_SRC) -o average_test $(LINKER_FLAGS)

clean:
	rm average_test

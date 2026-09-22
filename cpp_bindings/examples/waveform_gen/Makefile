# C parameters
CC = g++
CFLAGS = -Wall -O3

# Include the CPP wrapper makefile
CPP_WRAPPER_ROOT_DIR = ../../../
include ../../cpp_wrapper.mk

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

spectrum:
	$(CC) $(CFLAGS) waveform_gen_test.cpp $(CPP_WRAPPER_SRC) -o waveform_gen_test $(LINKER_FLAGS)

clean:
	rm waveform_gen_test
	rm results/*.bin

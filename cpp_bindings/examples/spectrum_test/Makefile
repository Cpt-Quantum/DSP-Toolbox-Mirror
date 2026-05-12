# Shared parameters for the decimation test
SAMPLE_RATE = 16000
DECIMATION_RATE = 4
INPUT_BIN = "results/input_data.bin"
OUTPUT_BIN = "results/decimated_data.bin"
REGEN_INPUT_FILE ?= false

# C parameters
CC = g++
CFLAGS = -Wall -O3

# Include the CPP wrapper makefile
CPP_WRAPPER_ROOT_DIR = ../../../
include ../../cpp_wrapper.mk

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

spectrum:
	$(CC) $(CFLAGS) spectrum_test.cpp $(CPP_WRAPPER_SRC) -o spectrum_test $(LINKER_FLAGS)

clean:
	rm spectrum_test
	rm results/*.bin

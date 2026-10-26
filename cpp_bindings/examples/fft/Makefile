# Shared parameters for the fft test
SAMPLE_RATE = 16000
INPUT_BIN = "results/input_data.bin"
OUTPUT_BIN = "results/fft_data.bin"

# C parameters
CC = g++
CFLAGS = -Wall -O3
DEFINES_LIST += F_S=$(SAMPLE_RATE)
CFLAGS += $(addprefix -D, $(DEFINES_LIST))

# Include the CPP wrapper makefile
CPP_WRAPPER_ROOT_DIR = ../../../
include ../../cpp_wrapper.mk

# Python parameters
PY = python3
PY_PLOT_FILE = plot_results.py
PY_PLOT_ARGS = --fs $(SAMPLE_RATE) --input $(INPUT_BIN) --output $(OUTPUT_BIN)

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

fft:
	$(CC) $(CFLAGS) fft_test.cpp $(CPP_WRAPPER_FFT_SRC) $(CPP_WRAPPER_WAVEFORM_SRC) -o fft_test $(LINKER_FLAGS)

run_all: fft
	./fft_test
	$(PY) $(PY_PLOT_FILE) $(PY_PLOT_ARGS)

plot:
	$(PY) $(PY_PLOT_FILE) $(PY_PLOT_ARGS)

clean:
	rm fft_test
	rm results/*.bin

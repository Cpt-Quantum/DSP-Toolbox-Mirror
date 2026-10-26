# Shared parameters for the multithreading test
SAMPLE_RATE = 16000
N_CHANNELS = 288 #256+32
N_THREADS = 4
CHUNK_SIZE = 100
FFT_SIZE = 1024
INPUT_BIN = "results/input_data.bin"
FIR_OUTPUT_BIN = "results/fir_output.bin"
IIR_SOS_OUTPUT_BIN = "results/iir_sos_output.bin"
IIR_BA_OUTPUT_BIN = "results/iir_ba_output.bin"
FFT_OUTPUT_BIN = "results/fft_output.bin"
FILTER_JSON = "filters.json"
REGEN_INPUT_FILE ?= false

# C parameters
CXX = g++
CFLAGS = -Wall -O3 -pthread
DEFINES_LIST += F_S=$(SAMPLE_RATE)
DEFINES_LIST += N_CHANNELS=$(N_CHANNELS)
DEFINES_LIST += N_THREADS=$(N_THREADS)
DEFINES_LIST += CHUNK_SIZE=$(CHUNK_SIZE)
DEFINES_LIST += FFT_SIZE=$(FFT_SIZE)
DEFINES_LIST += REGEN_INPUT_FILE=$(REGEN_INPUT_FILE)
CFLAGS += $(addprefix -D, $(DEFINES_LIST))

# Include the CPP wrapper makefile
CPP_WRAPPER_ROOT_DIR = ../../../
include ../../cpp_wrapper.mk

# Python parameters
PY = python3
PY_PLOT_FILE = plot_results.py
PY_PLOT_ARGS = --channels $(N_CHANNELS) --chunk $(CHUNK_SIZE) --fs $(SAMPLE_RATE)
PY_PLOT_ARGS += --filters $(FILTER_JSON) --input $(INPUT_BIN)
PY_PLOT_ARGS += --fir $(FIR_OUTPUT_BIN) --iir-sos $(IIR_SOS_OUTPUT_BIN)
PY_PLOT_ARGS += --iir-ba $(IIR_BA_OUTPUT_BIN)
PY_PLOT_ARGS += --fft $(FFT_OUTPUT_BIN) --fft-size $(FFT_SIZE)

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

multithread: $(CPP_WRAPPER_LIB)
	$(CXX) $(CFLAGS) multithread_test.cpp -o multithread_test $(LINKER_FLAGS)

design:
	$(PY) filter_design.py

run_all: multithread
	./multithread_test
	$(PY) $(PY_PLOT_FILE) $(PY_PLOT_ARGS)

plot:
	$(PY) $(PY_PLOT_FILE) $(PY_PLOT_ARGS)

clean:
	rm multithread_test
	rm results/*.bin

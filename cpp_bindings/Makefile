# Shared parameters for the decimation test
SAMPLE_RATE = 16000
N_CHANNELS = 288 #256+32
N_TAPS = 100
DECIMATION_RATE = 4
INPUT_BIN = "results/input_data.bin"
OUTPUT_BIN = "results/decimated_data.bin"
REGEN_INPUT_FILE ?= false

# C parameters
CC = g++
CFLAGS = -Wall -O3
DEFINES_LIST += F_S=$(SAMPLE_RATE)
DEFINES_LIST += N_CHANNELS=$(N_CHANNELS)
DEFINES_LIST += N_TAPS=$(N_TAPS)
DEFINES_LIST += DECIMATION=$(DECIMATION_RATE)
DEFINES_LIST += REGEN_INPUT_FILE=$(REGEN_INPUT_FILE)
CFLAGS += $(addprefix -D, $(DEFINES_LIST))

# Include the CPP wrapper makefile
include cpp_wrapper.mk

# Python parameters
PY = python3
PY_DATA_COMPARE_FILE = data_compare.py
PY_DATA_COMPARE_ARGS = --channels $(N_CHANNELS) --taps $(N_TAPS)
PY_DATA_COMPARE_ARGS += --decimation $(DECIMATION_RATE)
PY_DATA_COMPARE_ARGS += --input $(INPUT_BIN) --output $(OUTPUT_BIN)

# Set the linker flags
LINKER_FLAGS += $(CPP_WRAPPER_LDFLAGS)

decimation:
	$(CC) $(CFLAGS) decimation_test.cpp $(CPP_WRAPPER_SRC) -o decimation_test $(LINKER_FLAGS)

run_all: decimation
	./decimation_test
	$(PY) $(PY_DATA_COMPARE_FILE) $(PY_DATA_COMPARE_ARGS)

clean:
	rm decimation_test
	rm results/*.bin

# Shared parameters for the decimation test
SAMPLE_RATE = 16000
N_CHANNELS = 288 #256+32
N_TAPS = 32
DECIMATION_RATE = 4
INPUT_BIN = "results/input_data.bin"
OUTPUT_BIN = "results/decimated_data.bin"

# C parameters
CC = g++
CFLAGS = -Wall -O3
CFLAGS += -DF_S=$(SAMPLE_RATE) -DN_CHANNELS=$(N_CHANNELS)
CFLAGS += -DN_TAPS=$(N_TAPS) -DDECIMATION=$(DECIMATION_RATE)

C_SRC += ../inc/complex_float.c
C_SRC += ../inc/spectrum_gen_double.c
C_SRC += ../inc/spectrum_gen_float.c
C_SRC += ../inc/spectrum_gen_int32.c
C_SRC += ../inc/spectrum_gen_int64.c
C_SRC += ../inc/waveform_gen_double.c
C_SRC += ../inc/waveform_gen_float.c
C_SRC += ../inc/waveform_gen_int32.c
C_SRC += ../inc/waveform_gen_int64.c
C_SRC += ../cjson/cJSON.c
C_SRC += ../cjson/cJSON_helpers.c
C_SRC += ../filter/fir_double.c
C_SRC += ../filter/fir_float.c
C_SRC += ../filter/fir_int32.c
C_SRC += ../filter/fir_int64.c
C_SRC += ../filter/iir_double.c
C_SRC += ../filter/iir_float.c
C_SRC += ../filter/iir_int32.c
C_SRC += ../filter/iir_int64.c
C_SRC += ../filter/json_to_filt_double.c
C_SRC += ../filter/json_to_filt_float.c
C_SRC += ../filter/json_to_filt_int32.c
C_SRC += ../filter/json_to_filt_int64.c

LINKER_FLAGS = -lm

# Python parameters
PY = python3
PY_DATA_COMPARE_FILE = data_compare.py
PY_DATA_COMPARE_ARGS = --channels $(N_CHANNELS) --taps $(N_TAPS)
PY_DATA_COMPARE_ARGS += --decimation $(DECIMATION_RATE)
PY_DATA_COMPARE_ARGS += --input $(INPUT_BIN) --output $(OUTPUT_BIN)

decimation:
	$(CC) $(CFLAGS) decimation_test.cpp $(C_SRC) -o decimation_test $(LINKER_FLAGS)

run_all: decimation
	./decimation_test
	$(PY) $(PY_DATA_COMPARE_FILE) $(PY_DATA_COMPARE_ARGS)

clean:
	rm decimation_test
	rm results/*.bin

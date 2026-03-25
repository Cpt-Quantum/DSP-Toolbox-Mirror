# Shared variables
RESULTS_DIR = results
BIN_NAME = main.o
MAIN_INC = main_inc

# C variables
INC_DIR = ../inc
CC = gcc
CFLAGS = -Wall
LFLAGS = -lm

C_FILES = main.c
C_FILES += iir_float.c
C_FILES += fir_float.c
C_FILES += $(MAIN_INC).c
C_FILES += $(INC_DIR)/waveform_gen_float.c
C_FILES += $(INC_DIR)/spectrum_gen_float.c

# Python script variables
PYTHON = python3
SCRIPTS_DIR = scripts
FILTER_GEN_SCRIPT = $(SCRIPTS_DIR)/filter_design.py
FFT_SCRIPT = $(SCRIPTS_DIR)/fft_export.py
INPUT_DATA = $(RESULTS_DIR)/input_data.csv
FILTER_CSVS = $(RESULTS_DIR)/iir_simple_data.csv \
	$(RESULTS_DIR)/iir_sos_data.csv \
	$(RESULTS_DIR)/fir_data.csv \
	$(RESULTS_DIR)/fir_decimation_data.csv
MAT_FILES = $(FILTER_CSVS:.csv=.mat)
PNG_FILES = $(FILTER_CSVS:.csv=.png)

all: $(PNG_FILES)

# Generate the filter design and C includes
$(MAIN_INC):
	@echo "Generating the filter design outputs"
	$(PYTHON) $(FILTER_GEN_SCRIPT)

# Compile the C file
$(BIN_NAME): $(C_FILES)
	@echo "Compiling C filter implementation..."
	$(CC) $(CFLAGS) $(C_FILES) -o $(BIN_NAME) $(LFLAGS)

# Run the C binary to get the filter outputs and data in
$(FILTER_CSVS) $(INPUT_DATA): $(BIN_NAME)
	@echo "Running simulation to generate CSV data..."
	mkdir -p $(RESULTS_DIR)
	./$(BIN_NAME)

# Pattern match data files to run with the FFT script
$(RESULTS_DIR)/%.mat $(RESULTS_DIR)/%.png: $(RESULTS_DIR)/%.csv $(INPUT_DATA) $(FFT_SCRIPT)
	@echo "Processing FFT for $<..."
	$(PYTHON) $(FFT_SCRIPT) $(INPUT_DATA) $<

# Derived make rules depending on the rules above
# C binary generation rules (filter gives normal binary, debug creates binary with debug symbols)
filter: $(BIN_NAME)
debug: 
	$(CC) $(CFLAGS) -g $(C_FILES) -o $(BIN_NAME) $(LFLAGS)
# Valgrind test rule for finding memory bugs
leak-check:
	$(MAKE) debug
	mkdir -p $(RESULTS_DIR)
	valgrind --leak-check=full \
	  --show-leak-kinds=all \
	  --track-origins=yes \
	  ./$(BIN_NAME)


# Run through the whole pipeline
test: all
	@echo "All filters processed and plotted."

clean:
	rm -f $(BIN_NAME)
	rm -rf $(RESULTS_DIR)/*.csv $(RESULTS_DIR)/*.mat $(RESULTS_DIR)/*.png

.PHONY: all filter debug leak-check test clean

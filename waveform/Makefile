# Shared variables
RESULTS_DIR = results
BIN_NAME = main.o

# C variables
INC_DIR = ../inc
CC = gcc
CFLAGS = -Wall
LFLAGS = -lm

C_FILES = main.c
C_FILES += $(INC_DIR)/complex_float.c
C_FILES += $(INC_DIR)/waveform_gen_float.c

# Python script variables
PYTHON = python3
SCRIPTS_DIR = scripts
PLOT_SCRIPT = $(SCRIPTS_DIR)/plot_results.py
DATA_CSVS = $(RESULTS_DIR)/waveform_data.csv
PNG_FILE = $(RESULTS_DIR)/waveform.png

all: $(PNG_FILE)

# Compile the C file
$(BIN_NAME): $(C_FILES)
	@echo "Compiling C waveform generation..."
	$(CC) $(CFLAGS) $(C_FILES) -o $(BIN_NAME) $(LFLAGS)

# Run the C binary to get the waveforms
$(DATA_CSVS): $(BIN_NAME)
	@echo "Running the waveform generation to generate CSV data..."
	mkdir -p $(RESULTS_DIR)
	./$(BIN_NAME)

# Compare the waveforms against scipy and plot them
$(PNG_FILE): $(DATA_CSVS) $(PLOT_SCRIPT)
	@echo "Plotting the waveforms..."
	$(PYTHON) $(PLOT_SCRIPT) $(RESULTS_DIR)

# Derived make rules depending on the rules above
# C binary generation rules (waveform gives normal binary, debug creates binary with debug symbols)
waveform: $(BIN_NAME)
debug: $(C_FILES)
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
	@echo "Waveforms generated and plotted."

clean:
	rm -f $(BIN_NAME)
	rm -rf $(RESULTS_DIR)/*.csv $(RESULTS_DIR)/*.png

.PHONY: all waveform debug leak-check test clean

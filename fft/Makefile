# Shared variables
RESULTS_DIR = results
BIN_NAME = main.o

# C variables
CC = gcc
CFLAGS = -Wall
# The library built by the top level Makefile
ROOT_DIR = ..
LIB = $(ROOT_DIR)/build/libdsptoolbox.a
LFLAGS = $(LIB) -lm

C_FILES = main.c

# Python script variables
PYTHON = python3
SCRIPTS_DIR = scripts
PLOT_SCRIPT = $(SCRIPTS_DIR)/plot_results.py
DATA_CSVS = $(RESULTS_DIR)/input_data.csv \
	$(RESULTS_DIR)/fft_data.csv \
	$(RESULTS_DIR)/ifft_data.csv \
	$(RESULTS_DIR)/rfft_data.csv \
	$(RESULTS_DIR)/irfft_data.csv
PNG_FILE = $(RESULTS_DIR)/fft.png

all: $(PNG_FILE)

# Compile the C file
$(BIN_NAME): $(C_FILES) $(LIB)
	@echo "Compiling C FFT implementation..."
	$(CC) $(CFLAGS) $(C_FILES) -o $(BIN_NAME) $(LFLAGS)

# Run the C binary to get the transforms and data in
$(DATA_CSVS): $(BIN_NAME)
	@echo "Running the transforms to generate CSV data..."
	mkdir -p $(RESULTS_DIR)
	./$(BIN_NAME)

# Compare the transforms against numpy and plot them
$(PNG_FILE): $(DATA_CSVS) $(PLOT_SCRIPT)
	@echo "Plotting the transforms..."
	$(PYTHON) $(PLOT_SCRIPT) $(RESULTS_DIR)

# Build the library, or bring it up to date. It isn't touched if it is
# already up to date, so the binary is only relinked when it changes
$(LIB): FORCE
	$(MAKE) -C $(ROOT_DIR) lib

# Derived make rules depending on the rules above
# C binary generation rules (fft gives normal binary, debug creates binary with debug symbols)
fft: $(BIN_NAME)
debug: $(C_FILES) $(LIB)
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
	@echo "FFT processed and plotted."

clean:
	rm -f $(BIN_NAME)
	rm -rf $(RESULTS_DIR)/*.csv $(RESULTS_DIR)/*.png

FORCE:

.PHONY: all fft debug leak-check test clean FORCE

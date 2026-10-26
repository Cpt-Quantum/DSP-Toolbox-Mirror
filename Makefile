# Top level build for the DSP toolbox
#
# make                 Generates the C sources and builds build/libdsptoolbox.a
# make generate        Only generates the C sources
# make examples        Builds the C and C++ examples against the library
# make clean           Removes the build directory
#
# Link the library with -lm, unless every module used avoids the maths library
# (see the generation options). The variables below can be set on the command
# line, e.g. for an MCU:
#
# make CC=arm-none-eabi-gcc AR=arm-none-eabi-ar \
#      CFLAGS="-O2 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard" \
#      GEN_ARGS="--no-stdlib --fft-twiddle-table 1024 --fft-table-only" JSON=0

# Options passed to scripts/generate_sources.py, the sources are regenerated
# whenever they change
GEN_ARGS ?=
# Include the JSON filter loading (json_to_filt_*) and cJSON, which need the C
# standard library. Set to 0 for targets without one
JSON ?= 1
CFLAGS ?= -Wall -O2
PYTHON ?= python3
BUILD_DIR ?= build

LIB = $(BUILD_DIR)/libdsptoolbox.a
OBJ_DIR = $(BUILD_DIR)/obj
GENERATOR = scripts/generate_sources.py
TEMPLATES = $(wildcard templates/*.j2)
# The generated .c files, written by the generator as GENERATED_SRC
MANIFEST = $(BUILD_DIR)/generated_sources.mk
# The GEN_ARGS the sources were last generated with, so changing them
# regenerates the sources. The sources are generated in the source tree and
# shared by every BUILD_DIR, so this is too
GEN_ARGS_FILE = build/gen_args

# The examples built by make examples, with the target that builds each
C_EXAMPLES = average fft filter nco waveform
CPP_EXAMPLES = average:average complex:complex decimation:decimation \
	fft:fft multithread:multithread spectrum_test:spectrum \
	waveform_gen:spectrum

all: lib

# The manifest is included, so make generates the sources before working out
# what to build. Skipped for clean, which shouldn't generate anything
ifeq ($(filter clean,$(MAKECMDGOALS)),)
include $(MANIFEST)
endif

LIB_SRC = $(GENERATED_SRC)
ifeq ($(JSON),1)
LIB_SRC += cjson/cJSON.c cjson/cJSON_helpers.c
else
LIB_SRC := $(filter-out filter/json_to_filt_%.c,$(LIB_SRC))
endif
LIB_OBJ = $(LIB_SRC:%.c=$(OBJ_DIR)/%.o)

# Only touched when GEN_ARGS changes, so it is otherwise older than the manifest
$(GEN_ARGS_FILE): FORCE
	@mkdir -p $(@D)
	@echo '$(GEN_ARGS)' | cmp -s - $@ || echo '$(GEN_ARGS)' > $@

$(MANIFEST): $(GENERATOR) $(TEMPLATES) $(GEN_ARGS_FILE)
	$(PYTHON) $(GENERATOR) $(GEN_ARGS) --manifest $@

generate: $(MANIFEST)

# The generator only rewrites sources that change, so the manifest is only
# needed before compiling, and the header dependencies come from -MMD
$(OBJ_DIR)/%.o: %.c | $(MANIFEST)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(LIB): $(LIB_OBJ)
	rm -f $@
	$(AR) rcs $@ $^

lib: $(LIB)

examples: lib
	for ex in $(C_EXAMPLES); do $(MAKE) -C $$ex $$ex || exit 1; done
	for ex in $(CPP_EXAMPLES); do \
		$(MAKE) -C cpp_bindings/examples/$${ex%%:*} $${ex##*:} || exit 1; \
	done

clean:
	rm -rf $(BUILD_DIR)

-include $(LIB_OBJ:.o=.d)

FORCE:

.PHONY: all generate lib examples clean FORCE

# CPP wrapper makefile
# Include this to build the necessary files for using cpp_wrapper.h in your
# project

# Set the root directory for the project
CPP_WRAPPER_ROOT_DIR ?= ..

# Define the .c files needed
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/complex_float.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_double.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_float.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_int32.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_int64.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_double.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_float.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_int32.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_int64.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/cjson/cJSON.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/cjson/cJSON_helpers.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_double.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_float.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_int32.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_int64.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_double.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_float.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_int32.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_int64.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_double.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_float.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_int32.c
CPP_WRAPPER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_int64.c

# Set needed linker flags
CPP_WRAPPER_LDFLAGS += -lm


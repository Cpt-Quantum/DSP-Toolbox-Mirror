# CPP wrapper makefile
# Include this to build the necessary files for using the C++ wrappers in your
# project. CPP_WRAPPER_SRC has the .c files for all of cpp_wrapper.h, or use
# CPP_WRAPPER_<MODULE>_SRC for only the modules you include, e.g.
# CPP_WRAPPER_FILTER_SRC for cpp_wrapper_filter.h

# Set the root directory for the project
CPP_WRAPPER_ROOT_DIR ?= ..

# Define the .c files needed by each module
# cpp_wrapper_average.h
CPP_WRAPPER_AVERAGE_SRC += $(CPP_WRAPPER_ROOT_DIR)/average/avg_double.c
CPP_WRAPPER_AVERAGE_SRC += $(CPP_WRAPPER_ROOT_DIR)/average/avg_float.c
CPP_WRAPPER_AVERAGE_SRC += $(CPP_WRAPPER_ROOT_DIR)/average/avg_int32.c
CPP_WRAPPER_AVERAGE_SRC += $(CPP_WRAPPER_ROOT_DIR)/average/avg_int64.c

# cpp_wrapper_complex.h
CPP_WRAPPER_COMPLEX_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/complex_double.c
CPP_WRAPPER_COMPLEX_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/complex_float.c
CPP_WRAPPER_COMPLEX_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/complex_int32.c
CPP_WRAPPER_COMPLEX_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/complex_int64.c

# cpp_wrapper_fft.h
CPP_WRAPPER_FFT_SRC += $(CPP_WRAPPER_ROOT_DIR)/fft/fft_double.c
CPP_WRAPPER_FFT_SRC += $(CPP_WRAPPER_ROOT_DIR)/fft/fft_float.c

# cpp_wrapper_filter.h
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/cjson/cJSON.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/cjson/cJSON_helpers.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_double.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_float.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_int32.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/fir_int64.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_double.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_float.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_int32.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/iir_int64.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_double.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_float.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_int32.c
CPP_WRAPPER_FILTER_SRC += $(CPP_WRAPPER_ROOT_DIR)/filter/json_to_filt_int64.c

# cpp_wrapper_waveform.h
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/nco_double.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/nco_float.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/nco_int32.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/nco_int64.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_double.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_float.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_int32.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/spectrum_gen_int64.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_double.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_float.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_int32.c
CPP_WRAPPER_WAVEFORM_SRC += $(CPP_WRAPPER_ROOT_DIR)/inc/waveform_gen_int64.c

# All of the modules, for cpp_wrapper.h
CPP_WRAPPER_SRC += $(CPP_WRAPPER_AVERAGE_SRC)
CPP_WRAPPER_SRC += $(CPP_WRAPPER_COMPLEX_SRC)
CPP_WRAPPER_SRC += $(CPP_WRAPPER_FFT_SRC)
CPP_WRAPPER_SRC += $(CPP_WRAPPER_FILTER_SRC)
CPP_WRAPPER_SRC += $(CPP_WRAPPER_WAVEFORM_SRC)

# Set needed linker flags
CPP_WRAPPER_LDFLAGS += -lm

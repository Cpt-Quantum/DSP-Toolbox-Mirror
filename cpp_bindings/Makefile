CC = g++
CFLAGS = -Wall -g

C_SRC += ../inc/constants_float.c
C_SRC += ../inc/complex_float.c
C_SRC += ../inc/spectrum_gen_double.c
C_SRC += ../inc/spectrum_gen_float.c
C_SRC += ../inc/spectrum_gen_int32_t.c
C_SRC += ../inc/waveform_gen_double.c
C_SRC += ../inc/waveform_gen_float.c
C_SRC += ../inc/waveform_gen_int32_t.c
C_SRC += ../cjson/cJSON.c
C_SRC += ../cjson/cJSON_helpers.c
C_SRC += ../filter/fir_double.c
C_SRC += ../filter/fir_float.c
C_SRC += ../filter/fir_int32_t.c
C_SRC += ../filter/iir_double.c
C_SRC += ../filter/iir_float.c
C_SRC += ../filter/iir_int32_t.c
C_SRC += ../filter/json_to_filt_double.c
C_SRC += ../filter/json_to_filt_float.c
C_SRC += ../filter/json_to_filt_int32_t.c

LINKER_FLAGS = -lm

test:
	$(CC) $(CFLAGS) test.cpp $(C_SRC) -o test.o $(LINKER_FLAGS)
decimation:
	$(CC) $(CFLAGS) decimation_test.cpp $(C_SRC) -o decimation_test.o $(LINKER_FLAGS)

clean:
	rm *.o

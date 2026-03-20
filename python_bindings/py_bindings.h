#ifndef DSP_TOOLBOX_PY_BINDINGS_H
#define DSP_TOOLBOX_PY_BINDINGS_H

#include <stdlib.h>

#include "../inc/waveform_gen_float.h"

/* Expose functions for creating the needed C arrays from python */
void *create_c_float_arr(unsigned int arr_length);
void *create_c_wave_properties_arr(unsigned int arr_length);

/* Function to update the wave property array */
void write_wave_properties_arr(unsigned int index,
							   wave_properties_float_t *wave_properties_arr,
							   float amplitude, float offset, float frequency,
							   float phase, WAVEFORM_FUNC_FLOAT_E function_type);
/* Function to generate the waveform, this requires gen_time_float() has been called
 * first for the supplied time array */
void gen_waveform(wave_properties_float_t *wave_properties_arr, float *t, float *x,
				  unsigned int n_superpositions, unsigned int wave_length);
/* Expose the free function via this C function */
void free_c_arr(void *arr);

#endif

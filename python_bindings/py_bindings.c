#include <stdlib.h>

#include "../inc/waveform_gen.h"

/* Expose functions for creating the needed C arrays from python */
float *create_c_float_arr(unsigned int arr_length) {
	return (float *)malloc(arr_length * sizeof(float));
}
wave_properties_t *create_c_wave_properties_arr(unsigned int arr_length) {
	return (wave_properties_t *)malloc(arr_length * sizeof(wave_properties_t));
}

/* Function to update the wave property array */
void write_wave_properties_arr(unsigned int index,
							   wave_properties_t *wave_properties_arr,
							   float amplitude, float offset, float frequency,
							   float phase, WAVEFORM_FUNC_E function_type) {
	wave_properties_arr[index].amplitude = amplitude;
	wave_properties_arr[index].offset = offset;
	wave_properties_arr[index].frequency = frequency;
	wave_properties_arr[index].phase = phase;
	wave_properties_arr[index].function_type = function_type;
};
/* Function to generate the waveform, this requires gen_time() has been called
 * first for the supplied time array */
void gen_waveform(wave_properties_t *wave_properties_arr, float *t, float *x,
				  unsigned int n_superpositions, unsigned int wave_length) {
	/* Loop through each superposition to calculate the full waveform */
	for (unsigned int i = 0; i < n_superpositions; i++) {
		waveform_add_superposition(wave_properties_arr[i], t, x, wave_length);
	}
};

/* Expose the free function via this C function */
void free_c_arr(void *arr) { free(arr); };

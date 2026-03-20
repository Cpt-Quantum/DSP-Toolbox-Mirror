#include "cpp_wrapper.h"
#include <vector>

/* Constructors/destructors for the time class */
waveform_t::waveform_t(unsigned int _sample_rate, unsigned int _wave_length) {
	/* Set the internal variables */
	sample_rate = _sample_rate;
	wave_length = _wave_length;
	/* Create the time array and then calculate the time values */
	t.reserve(_wave_length);
	for (unsigned int i = 0; i < _wave_length; i++) {
		t.push_back(((float)i) / _sample_rate);
	}
};

/* Constructors/destructors for the data class */
waveform_x::waveform_x(unsigned int _wave_length) {
	/* Set the internal variables */
	wave_length = _wave_length;
	/* Setup the vector for storing the data elements */
	x.resize(_wave_length);
};
waveform_x::waveform_x(unsigned int _wave_length,
					   vector<wave_properties_float_t> _wave_properties) {
	/* Set the internal variables */
	wave_length = _wave_length;
	/* Setup the vector for storing the data elements */
	x.resize(_wave_length);
	/* Copy the wave properties provided by the constructor caller to the local
	 * storage */
	wave_properties = _wave_properties;
};

/* Data class function definitions */
void waveform_x::add_wave_properties(float amplitude, float offset,
									 float frequency, float phase,
									 WAVEFORM_FUNC_FLOAT_E function_type) {
	/* Create the temporary waveform properties struct and add it to the vector
	 * storage */
	wave_properties_float_t wave_properties_tmp = {.amplitude = amplitude,
											 .offset = offset,
											 .frequency = frequency,
											 .phase = phase,
											 .function_type = function_type};
	wave_properties.push_back(wave_properties_tmp);
};
void waveform_x::gen_waveform(float *t) {
	/* Get the number of superpositions in from the wave_properties vector */
	unsigned int n_superpositions = wave_properties.size();
	/* Loop through each superposition to calculate the full waveform */
	for (unsigned int i = 0; i < n_superpositions; i++) {
		waveform_add_superposition_float(wave_properties[i], t, x.data(), wave_length);
	}
};

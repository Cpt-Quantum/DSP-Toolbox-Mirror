#ifndef DSP_TOOLBOX_CPP_WRAPPER_H
#define DSP_TOOLBOX_CPP_WRAPPER_H

#include "../filter/fir_double.c"
#include "../filter/fir_double.h"
#include "../filter/fir_float.c"
#include "../filter/fir_float.h"
#include "../filter/fir_int32_t.c"
#include "../filter/fir_int32_t.h"
#include "../filter/iir_double.c"
#include "../filter/iir_double.h"
#include "../filter/iir_float.c"
#include "../filter/iir_float.h"
#include "../filter/iir_int32_t.c"
#include "../filter/iir_int32_t.h"
#include "../inc/spectrum_gen_double.h"
#include "../inc/spectrum_gen_float.h"
#include "../inc/spectrum_gen_int32_t.h"
#include "../inc/spectrum_gen_shared.h"
#include "../inc/waveform_gen_double.h"
#include "../inc/waveform_gen_float.h"
#include "../inc/waveform_gen_int32_t.h"
#include "../inc/waveform_gen_shared.h"

#include <cstdint>
#include <vector>

using namespace std;

template <typename num_t> class waveform_t {
  private:
	unsigned int sample_rate;
	unsigned int wave_length;

  public:
	vector<num_t> t;
	waveform_t(unsigned int _sample_rate, unsigned int _wave_length) {
		/* Set the internal variables */
		sample_rate = _sample_rate;
		wave_length = _wave_length;
		/* Create the time array and then calculate the time values */
		t.reserve(_wave_length);
		for (unsigned int i = 0; i < _wave_length; i++) {
			t.push_back(((num_t)i) / _sample_rate);
		}
	};
};

template <typename wave_properties_t, typename num_t> class waveform_x {
  private:
	unsigned int wave_length;
	vector<wave_properties_t> wave_properties;

	/* Create private member functions that will be selected based on num_t */
	inline void waveform_add_superposition(wave_properties_t wave_properties,
										   float *t, float *y, size_t length) {
		waveform_add_superposition_float(wave_properties, t, y, length);
	};
	inline void waveform_add_superposition(wave_properties_t wave_properties,
										   double *t, double *y,
										   size_t length) {
		waveform_add_superposition_double(wave_properties, t, y, length);
	};
	inline void waveform_add_superposition(wave_properties_t wave_properties,
										   int32_t *t, int32_t *y,
										   size_t length) {
		waveform_add_superposition_int32_t(wave_properties, t, y, length);
	};
	inline void gen_spectrum_type(float *t, float *y, size_t length,
								  uint32_t f_s, uint32_t num_steps,
								  AMPLITUDE_SPECTRUM_E profile) {
		generate_spectrum_waveform_float(t, y, length, f_s, num_steps, profile);
	};
	inline void gen_spectrum_type(double *t, double *y, size_t length,
								  uint32_t f_s, uint32_t num_steps,
								  AMPLITUDE_SPECTRUM_E profile) {
		generate_spectrum_waveform_double(t, y, length, f_s, num_steps,
										  profile);
	};
	inline void gen_spectrum_type(int32_t *t, int32_t *y, size_t length,
								  uint32_t f_s, uint32_t num_steps,
								  AMPLITUDE_SPECTRUM_E profile) {
		generate_spectrum_waveform_int32_t(t, y, length, f_s, num_steps,
										   profile);
	};

  public:
	vector<num_t> x;
	/* Functions to add new entries to any vector types */
	void add_wave_properties(num_t amplitude, num_t offset, num_t frequency,
							 num_t phase, WAVEFORM_FUNC_E function_type) {
		/* Create the temporary waveform properties struct and add it to the
		 * vector storage */
		wave_properties_t wave_properties_tmp = {.amplitude = amplitude,
												 .offset = offset,
												 .frequency = frequency,
												 .phase = phase,
												 .function_type =
													 function_type};
		wave_properties.push_back(wave_properties_tmp);
	};

	void gen_waveform(vector<num_t> &t) {
		/* Get the number of superpositions in from the wave_properties vector
		 */
		unsigned int n_superpositions = wave_properties.size();
		/* Loop through each superposition to calculate the full waveform */
		for (unsigned int i = 0; i < n_superpositions; i++) {
			waveform_add_superposition(wave_properties[i], t.data(), x.data(),
									   wave_length);
		}
	};

	void gen_spectrum(vector<num_t> &t, size_t length, uint32_t f_s,
					  uint32_t num_steps, AMPLITUDE_SPECTRUM_E profile) {
		gen_spectrum_type(t.data(), x.data(), length, f_s, num_steps, profile);
	};
	/* Constructors/destructors */
	waveform_x(unsigned int _wave_length) : wave_length(_wave_length) {
		/* Setup the vector for storing the data elements */
		x.resize(_wave_length);
	};

	waveform_x(unsigned int _wave_length,
			   vector<wave_properties_t> _wave_properties)
		: wave_length(_wave_length), wave_properties(_wave_properties) {
		/* Setup the vector for storing the data elements */
		x.resize(_wave_length);
	};
};

#endif /* DSP_TOOLBOX_CPP_WRAPPER_H */

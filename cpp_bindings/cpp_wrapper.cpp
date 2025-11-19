#include "cpp_wrapper.h"

/* Constructors/destructors for the time class */
waveform_t::waveform_t(unsigned int _sample_rate, unsigned int _wave_length) {
	/* Set the internal variables */
	sample_rate = _sample_rate;
	wave_length = _wave_length;
	/* Create the time array and then calculate the time values */
	t.reserve(_wave_length);
	for (unsigned int i = 0; i < _wave_length; i++) {
		t[i] = ((float)i) / _sample_rate;
	}
};

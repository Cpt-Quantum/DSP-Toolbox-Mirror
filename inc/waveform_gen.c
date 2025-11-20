#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "complex.h"
#include "constants.h"
#include "waveform_gen.h"

void gen_time(float *t, uint32_t length, uint32_t sample_freq) {
	for (uint32_t i = 0; i < length; i++) {
		t[i] = ((float)i) / sample_freq;
	}
}

void gen_freq(float *f, uint32_t length, uint32_t sample_freq) {
	for (uint32_t i = 0; i < length; i++) {
		f[i] = 0.5 * ((float)i) * sample_freq / length;
	}
}

/* Real valued function variants */
void add_sin(float *t, float *x, wave_properties_t wave_properties,
			 uint32_t length) {
	for (uint32_t i = 0; i < length; i++) {
		x[i] = x[i] + wave_properties.offset +
			   wave_properties.amplitude *
				   sinf(pi2 * wave_properties.frequency * t[i] +
						pi2 * wave_properties.phase);
	}
}

void add_cos(float *t, float *x, wave_properties_t wave_properties,
			 uint32_t length) {
	for (uint32_t i = 0; i < length; i++) {
		x[i] = x[i] + wave_properties.offset +
			   wave_properties.amplitude *
				   cosf(pi2 * wave_properties.frequency * t[i] +
						pi2 * wave_properties.phase);
	}
}

/* Complex valued function variants */
void add_sin_complex(float *t, complex_t *x, bool write_real, bool write_imag,
					 wave_properties_t wave_properties, uint32_t length) {
	if (write_real) {
		for (uint32_t i = 0; i < length; i++) {
			x[i].real = x[i].real + wave_properties.offset +
						wave_properties.amplitude *
							sinf(pi2 * wave_properties.frequency * t[i] +
								 pi2 * wave_properties.phase);
		}
	}
	if (write_imag) {
		for (uint32_t i = 0; i < length; i++) {
			x[i].imag = x[i].imag + wave_properties.offset +
						wave_properties.amplitude *
							sinf(pi2 * wave_properties.frequency * t[i] +
								 pi2 * wave_properties.phase);
		}
	}
}

void add_cos_complex(float *t, complex_t *x, bool write_real, bool write_imag,
					 wave_properties_t wave_properties, uint32_t length) {
	if (write_real) {
		for (uint32_t i = 0; i < length; i++) {
			x[i].real = x[i].real + wave_properties.offset +
						wave_properties.amplitude *
							cosf(pi2 * wave_properties.frequency * t[i] +
								 pi2 * wave_properties.phase);
		}
	}
	if (write_imag) {
		for (uint32_t i = 0; i < length; i++) {
			x[i].imag = x[i].imag + wave_properties.offset +
						wave_properties.amplitude *
							cosf(pi2 * wave_properties.frequency * t[i] +
								 pi2 * wave_properties.phase);
		}
	}
}

void waveform_add_superposition(wave_properties_t wave_properties,
									   float *t, float *x,
									   uint32_t data_length) {
	switch (wave_properties.function_type) {
	case FUNCTION_SINE:
		add_sin(t, x, wave_properties, data_length);
		break;
	case FUNCTION_COS:
		add_cos(t, x, wave_properties, data_length);
		break;
	default:
		break;
	}
}

void waveform_add_superposition_complex(wave_properties_t wave_properties, float *t,
								   complex_t *x, bool write_real,
								   bool write_imag, int32_t data_length) {
	switch (wave_properties.function_type) {
	case FUNCTION_SINE:
		add_sin_complex(t, x, write_real, write_imag, wave_properties,
						data_length);
		break;
	case FUNCTION_COS:
		add_cos_complex(t, x, write_real, write_imag, wave_properties,
						data_length);
		break;
	default:
		break;
	}
}

void init_waveform(wave_settings_t wave_settings, float *t, float *x) {
	/* Generate the time according to settings first */
	gen_time(t, wave_settings.data_length, wave_settings.fs);
	/* Now generate the main waveform */
	for (uint8_t i = 0; i < wave_settings.num_superpositions; i++) {
		waveform_add_superposition(wave_settings.superposition[i], t, x,
								   wave_settings.data_length);
	}
	/* Add in the DC offset for the signal */
	for (uint32_t i = 0; i < wave_settings.data_length; i++) {
		t[i] += wave_settings.dc_offset;
	}
}

void init_waveform_complex(wave_settings_complex_t wave_settings, float *t,
						   complex_t *x) {
	/* Generate the time according to settings first */
	gen_time(t, wave_settings.data_length, wave_settings.fs);
	/* Now generate the main waveform */
	for (uint8_t i = 0; i < wave_settings.num_superpositions; i++) {
		/* Add to the real parts based on the waveform settings */
		waveform_add_superposition_complex(wave_settings.superposition_real[i],
										   t, x, true, false,
										   wave_settings.data_length);
		/* Add to the imaginary parts based on the waveform settings */
		waveform_add_superposition_complex(wave_settings.superposition_imag[i],
										   t, x, false, true,
										   wave_settings.data_length);
	}
	/* Add in the DC offset for the signal */
	for (uint32_t i = 0; i < wave_settings.data_length; i++) {
		x[i].real += wave_settings.dc_offset.real;
		x[i].imag += wave_settings.dc_offset.imag;
	}
}

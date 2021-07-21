#include <math.h>
#include <stdint.h>

#include "waveform_gen.h"
#include "complex.h"
#include "constants.h"

void gen_time(float *t, uint32_t length, uint32_t sample_freq)
{
	for (uint32_t i = 0; i < length; i++)
	{
		t[i] = ((float)i) / sample_freq;
	}
}

void gen_freq(float *f, uint32_t length, uint32_t sample_freq)
{
	for (uint32_t i = 0; i < length; i++)
	{
		f[i] = 0.5 * ((float)i) * sample_freq / length;
	}
}

/* Real valued function variants */
void add_sin(float *x, float *y,
				wave_properties_t wave_properties, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i] = y[i] + wave_properties.offset
				+ wave_properties.amplitude
					* sinf(pi2 * wave_properties.frequency * x[i] 
							+ pi2 * wave_properties.phase);
	}
}

void add_cos(float *x, float *y,
				wave_properties_t wave_properties, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i] = y[i] + wave_properties.offset
				+ wave_properties.amplitude
					* cosf(pi2 * wave_properties.frequency * x[i]
							 + pi2 * wave_properties.phase);
	}
}

/* Complex valued function variants */
void add_sin_real(float *x, complex_t *y,
					wave_properties_t wave_properties, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].real = y[i].real + wave_properties.offset
					+ wave_properties.amplitude
					* sinf(pi2 * wave_properties.frequency * x[i]
							+ pi2 * wave_properties.phase);
	}
}

void add_cos_real(float *x, complex_t *y,
					wave_properties_t wave_properties, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].real = y[i].real + wave_properties.offset
					+ wave_properties.amplitude
						* cosf(pi2 * wave_properties.frequency * x[i]
								+ pi2 * wave_properties.phase);
	}
}

void add_sin_imag(float *x, complex_t *y,
					wave_properties_t wave_properties, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].imag = y[i].imag + wave_properties.offset
					+ wave_properties.amplitude
						* sinf(pi2 * wave_properties.frequency * x[i]
								+ pi2 * wave_properties.phase);
	}
}

void add_cos_imag(float *x, complex_t *y,
					wave_properties_t wave_properties, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].imag = y[i].imag + wave_properties.offset
					+ wave_properties.amplitude
						* cosf(pi2 * wave_properties.frequency * x[i]
								+ pi2 * wave_properties.phase);
	}
}

void init_waveform(wave_settings_t wave_settings, float *t, float *x)
{
	/* Generate the time according to settings first */
	gen_time(t, wave_settings.data_length, wave_settings.fs);
	/* Now generate the main waveform */
	for (uint8_t i = 0; i < wave_settings.num_superpositions; i++)
	{
		switch (wave_settings.superposition[i].function_type)
		{
			case FUNCTION_SINE:
				add_sin(t, x,
							wave_settings.superposition[i],
							wave_settings.data_length);
				break;
			case FUNCTION_COS:
				add_cos(t, x,
							wave_settings.superposition[i],
							wave_settings.data_length);
				break;
			default:
				break;
		}
	}
	/* Add in the DC offset for the signal */
	for (uint32_t i = 0; i < wave_settings.data_length; i++)
	{
		x[i] += wave_settings.dc_offset;
	}
}


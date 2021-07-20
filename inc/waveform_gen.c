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
void add_sin(float *x, float *y, uint32_t length, uint32_t frequency,
				float amplitude, float offset, float phase)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i] = y[i] + offset
				+ amplitude * sin(pi2 * ((float)frequency) * x[i] + pi2 * phase);
	}
}

void add_cos(float *x, float *y, uint32_t length, uint32_t frequency,
				float amplitude, float offset, float phase)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i] = y[i] + offset
				+ amplitude * cos(pi2 * ((float)frequency) * x[i] + pi2 * phase);
	}
}

/* Complex valued function variants */
void add_sin_real(float *x, complex_t *y, uint32_t length, uint32_t frequency,
					float amplitude, float offset, float phase)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].real = y[i].real + offset
					+ amplitude * sin(pi2 * frequency * x[i] + pi2 * phase);
	}
}

void add_cos_real(float *x, complex_t *y, uint32_t length, uint32_t frequency,
					float amplitude, float offset, float phase)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].real = y[i].real + offset
					+ amplitude * cos(pi2 * frequency * x[i] + pi2 * phase);
	}
}

void add_sin_imag(float *x, complex_t *y, uint32_t length, uint32_t frequency,
					float amplitude, float offset, float phase)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].imag = y[i].imag + offset
					+ amplitude * sin(pi2 * frequency * x[i] + pi2 * phase);
	}
}

void add_cos_imag(float *x, complex_t *y, uint32_t length, uint32_t frequency,
					float amplitude, float offset, float phase)
{
	for (uint32_t i = 0; i < length; i++)
	{
		y[i].imag = y[i].imag + offset
					+ amplitude * cos(pi2 * frequency * x[i] + pi2 * phase);
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
							wave_settings.data_length,
							wave_settings.superposition[i].frequency,
							wave_settings.superposition[i].amplitude,
							wave_settings.superposition[i].offset,
							wave_settings.superposition[i].phase);
			case FUNCTION_COS:
				add_cos(t, x,
							wave_settings.data_length,
							wave_settings.superposition[i].frequency,
							wave_settings.superposition[i].amplitude,
							wave_settings.superposition[i].offset,
							wave_settings.superposition[i].phase);
			default:
				break;
		}
	}
}


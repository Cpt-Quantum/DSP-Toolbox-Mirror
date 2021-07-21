#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "../inc/constants.h"
#include "../inc/waveform_gen.h"

/* Define the input waveforms */
/* Waveform 1 */
#define WAVE_1_DC_OFFSET (3.3/2)
static const wave_properties_t wave_prop_1[] = {
	[0] = {
		.amplitude = 1,
		.offset = 0,
		.frequency = 80,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
	[1] = {
		.amplitude = 0.8,
		.offset = 0,
		.frequency = 200,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
	[2] = {
		.amplitude = 1.2,
		.offset = 0,
		.frequency = 1000,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
	[3] = {
		.amplitude = 0.9,
		.offset = 0,
		.frequency = 4000,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
	[4] = {
		.amplitude = 0.5,
		.offset = 0,
		.frequency = 8000,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
};
#define WAVE_1_SUP_COUNT (sizeof(wave_prop_1)/sizeof(wave_prop_1[0]))
#define WAVE_1_FS 40000
#define WAVE_1_LENGTH (WAVE_1_FS * 1)
static const wave_settings_t wave_sett_1 = {
	.fs = WAVE_1_FS,
	.data_length = WAVE_1_LENGTH,
	.dc_offset = WAVE_1_DC_OFFSET,
	.superposition = wave_prop_1,
	.num_superpositions = WAVE_1_SUP_COUNT
};

/* Arrays for wave 1 */
float t_1[WAVE_1_LENGTH] = {0};
float x_1[WAVE_1_LENGTH] = {0};

static float average(float *data_in, uint32_t data_length)
{
	float avg = 0;
	for (uint32_t i = 0; i < data_length; i++)
	{
		avg += data_in[i];
	}
	avg = avg / data_length;
	return avg;
}

static float power_average(float *data_in, uint32_t data_length)
{
	float power = 0;
	for (uint32_t i = 0; i < data_length; i++)
	{
		power += data_in[i] * data_in[i];
	}
	return (power/data_length);
}

static const char input_filename[] = "input_file.csv";
static const char output_filename[] = "output_file.csv";
static const char power_filename[] = "power.csv";

int main(void)
{
	init_waveform(wave_sett_1, t_1, x_1);

	/* Print the input waveform out to file */
	FILE *ifile = fopen(input_filename, "w");

	for (uint32_t i = 0; i < WAVE_1_LENGTH; i++)
	{
		fprintf(ifile, "%f,%f\n", t_1[i], x_1[i]);
	}

	fclose(ifile);
	
	/* Remove the DC offset of the signal */
#define DETREND_TIME (WAVE_1_FS/1)
	uint32_t data_chunks = wave_sett_1.data_length / DETREND_TIME;
	for (uint32_t i = 0; i < data_chunks; i++)
	{
		uint32_t offset = DETREND_TIME * i;
		float avg = average(&x_1[offset], DETREND_TIME);
		for (uint32_t j = offset; j < (DETREND_TIME + offset); j++)
		{
			x_1[j] = x_1[j] - avg;
		}
		/* Calculate the average power in each section */
		float power_avg = power_average(&x_1[offset], DETREND_TIME);
	}
	
	/* Print the output waveform out to file */
	FILE *ofile = fopen(output_filename, "w");

	for (uint32_t i = 0; i < WAVE_1_LENGTH; i++)
	{
		fprintf(ofile, "%f,%f\n", t_1[i], x_1[i]);
	}

	fclose(ofile);

}

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "../inc/waveform_gen_float.h"
#include "avg.h"

/* Define the input waveforms */
/* Waveform 1 */
#define WAVE_1_DC_OFFSET (3.3 / 2)
static const wave_properties_float_t wave_prop_1[] = {
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
#define WAVE_1_SUP_COUNT (sizeof(wave_prop_1) / sizeof(wave_prop_1[0]))
#define WAVE_1_FS 40000
#define WAVE_1_LENGTH (WAVE_1_FS * 2)
static const wave_settings_float_t wave_sett_1 = {
	.fs = WAVE_1_FS,
	.data_length = WAVE_1_LENGTH,
	.dc_offset = WAVE_1_DC_OFFSET,
	.superposition = wave_prop_1,
	.num_superpositions = WAVE_1_SUP_COUNT};

/* Arrays for wave 1 */
float t_1[WAVE_1_LENGTH] = {0};
float x_1[WAVE_1_LENGTH] = {0};

/* y_1 corresponds to the detrended signal done via average, y_2 corresponds to the detrended signal */
/* done via moving average. */
float y_1[WAVE_1_LENGTH] = {0};
float y_2[WAVE_1_LENGTH] = {0};

/* Moving average definitions for signal detrending */
#define MOVING_AVG_CHUNK_LENGTH 50
#define DETREND_SAMPLES (WAVE_1_FS / (10 * MOVING_AVG_CHUNK_LENGTH))
float dc_offset_avg_buff[DETREND_SAMPLES] = {0};
moving_avg_t detrend_avg = {
	.data_buff = dc_offset_avg_buff,
	.data_buff_length = DETREND_SAMPLES,
	.current_total = 0,
	.head = 0,
	.tail = 0,
	.first_cycle = true};

static const char input_filename[] = "input_file.csv";
static const char output_filename[] = "output_file.csv";
static const char power_filename[] = "power.csv";

int main(void)
{
	init_waveform_float(wave_sett_1, t_1, x_1);

	/* Print the input waveform out to file */
	FILE *ifile = fopen(input_filename, "w");

	for (uint32_t i = 0; i < WAVE_1_LENGTH; i++)
	{
		fprintf(ifile, "%f,%f\n", t_1[i], x_1[i]);
	}

	fclose(ifile);

	/* Remove the DC offset of the signal */
#define DETREND_TIME (WAVE_1_FS / 10)
	uint32_t data_chunks = wave_sett_1.data_length / DETREND_TIME;
	for (uint32_t i = 0; i < data_chunks; i++)
	{
		uint32_t offset = DETREND_TIME * i;
		float avg = average(&x_1[offset], DETREND_TIME);
		for (uint32_t j = offset; j < (DETREND_TIME + offset); j++)
		{
			y_1[j] = x_1[j] - avg;
		}
	}
	for (uint32_t i = 0; i < WAVE_1_LENGTH / MOVING_AVG_CHUNK_LENGTH; i++)
	{
		uint32_t offset = MOVING_AVG_CHUNK_LENGTH * i;
		float avg = average(&x_1[offset], MOVING_AVG_CHUNK_LENGTH);
		float moving_avg = moving_average(&detrend_avg, avg);
		for (uint32_t j = 0; j < MOVING_AVG_CHUNK_LENGTH; j++)
		{
			y_2[offset + j] = x_1[offset + j] - moving_avg;
		}
	}

	/* Calculate the power over the signal in chunks corresponding to 1kHz */
#define POWER_AVG_TIME (WAVE_1_FS / 1000)
	data_chunks = wave_sett_1.data_length / POWER_AVG_TIME;
	for (uint32_t i = 0; i < data_chunks; i++)
	{
		float power_avg = power_average(&y_1[i * POWER_AVG_TIME], POWER_AVG_TIME);
		printf("Power = %f\n", power_avg);
	}

	/* Print the output waveform out to file */
	FILE *ofile = fopen(output_filename, "w");

	for (uint32_t i = 0; i < WAVE_1_LENGTH; i++)
	{
		fprintf(ofile, "%f,%f,%f\n", t_1[i], y_1[i], y_2[i]);
	}

	fclose(ofile);
}

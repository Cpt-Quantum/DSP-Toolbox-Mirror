#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "../inc/constants.h"
#include "../inc/waveform_gen.h"

/* Define the input waveforms */
/* Waveform 1 */
static const wave_properties_t wave_prop_1[] = {
	[0] = {
		.amplitude = 10,
		.offset = 0,
		.frequency = 20,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
	[1] = {
		.amplitude = 100,
		.offset = 0,
		.frequency = 100,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
	[2] = {
		.amplitude = 200,
		.offset = 0,
		.frequency = 200,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
	[3] = {
		.amplitude = 10,
		.offset = 0,
		.frequency = 400,
		.phase = 0,
		.function_type = FUNCTION_SINE,
	},
};
#define WAVE_1_SUP_COUNT (sizeof(wave_prop_1)/sizeof(wave_prop_1[0]))
#define WAVE_1_LENGTH 10000
static const wave_settings_t wave_sett_1 = {
	.fs = 1000,
	.data_length = WAVE_1_LENGTH,
	.superposition = wave_prop_1,
	.num_superpositions = WAVE_1_SUP_COUNT
};

/* Arrays for wave 1 */
float t_1[WAVE_1_LENGTH];
float x_1[WAVE_1_LENGTH];

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

static const char input_filename[] = "input_file.csv";

int main(void)
{
	init_waveform(wave_sett_1, t_1, x_1);

	/* Print the waveform out to file */
	FILE *ifile = fopen(input_filename, "w");

	for (uint32_t i = 0; i < WAVE_1_LENGTH; i++)
	{
		fprintf(ifile, "%f,%f\n", t_1[i], x_1[i]);
	}

	fclose(ifile);

	/* Calculate the averages */
	float avg_1s = average(&x_1[0], 1000);
	float avg_100ms = average(&x_1[0], 100);
	float avg_10ms = average(&x_1[0], 10);
#define OFFSET 500
	float offset_avg_1s = average(&x_1[OFFSET], 1000);
	float offset_avg_100ms = average(&x_1[OFFSET], 100);
	float offset_avg_10ms = average(&x_1[OFFSET], 10);

	/* Print out the averages */
	printf("0 offset averages: 1s = %f, 100ms = %f, 10ms = %f\n", 
			avg_1s, avg_100ms, avg_10ms);
	printf("Offset averages: 1s = %f, 100ms = %f, 10ms = %f\n", 
			offset_avg_1s, offset_avg_100ms, offset_avg_10ms);

}

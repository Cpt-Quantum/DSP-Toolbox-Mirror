#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "iir.h"
#include "../inc/constants.h"
#include "../inc/complex.h"
#include "../inc/waveform_gen.h"

/* Define the frequency at which the waveform is sampled */
#define SAMPLING_FREQUENCY 512

/* Define the length of the data to work on */
#define DATA_CHUNK_SIZE 16
#define DATA_CHUNK_COUNT (SAMPLING_FREQUENCY * 1)
#define DATA_SIZE (DATA_CHUNK_SIZE * DATA_CHUNK_COUNT)

/* Define the parameters of the input waveform */
wave_properties_t wave_1[] = {
	[0] = {.amplitude = 10, .offset = 0, .frequency = 5, .phase = 0, .function_type = FUNCTION_COS},
	[1] = {.amplitude = 50, .offset = 0, .frequency = 50, .phase = 0, .function_type = FUNCTION_COS},
	[2] = {.amplitude = 50, .offset = 0, .frequency = 1, .phase = 0, .function_type = FUNCTION_COS},
	[3] = {.amplitude = 50, .offset = 0, .frequency = 100, .phase = 0, .function_type = FUNCTION_COS},
};
wave_settings_t wave_1_settings = {
	.fs = SAMPLING_FREQUENCY,
	.data_length = DATA_SIZE,
	.dc_offset = 0,
	.superposition = wave_1,
	.num_superpositions = sizeof(wave_1) / sizeof(wave_1[0]),
};

/* Define the filenames for the input data file and the output data file */
const char in_filename[] = "input_data.csv";
const char out_filename[] = "output_data.csv";

/* Define IIR filter parameters */
float filt_a[] = {1.0, -3.187435145652140, 4.437529217427072, -3.025620552651779, 0.901108152734037};
float filt_b[] = {0.949266292642437, -3.106527849151959, 4.440104784876235, -3.106527849151959, 0.949266292642437};
#define FILT_TAPS (sizeof(filt_a) / sizeof(filt_a[0]))
float filt_in_buff[FILT_TAPS] = {0};
float filt_out_buff[FILT_TAPS] = {0};

struct iir_coeff_t iir_coeffs = {
	.a = filt_a,
	.b = filt_b};

struct iir_t filt = {
	.iir_coeff = &iir_coeffs,
	.in_buff = filt_in_buff,
	.out_buff = filt_out_buff,
	.taps = FILT_TAPS,
	.cycle = 0};

int main()
{
	float t[DATA_SIZE];
	float x[DATA_SIZE];
	float y[DATA_SIZE];

	/* Initialise the arrays to zero */
	for (uint32_t i = 0; i < DATA_SIZE; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}

	/* Generate time and data */
	gen_time(t, DATA_SIZE, SAMPLING_FREQUENCY);
	init_waveform(wave_1_settings, t, x);

	/* Perform the filter */
	/* The data is sent through in chunks to simulate live data */
	for (uint32_t i = 0; i < DATA_CHUNK_COUNT; i++)
	{
		uint32_t start_index = i * DATA_CHUNK_SIZE;
		//iir_filter(&x[start_index], &y[start_index], &filt, DATA_CHUNK_SIZE);
		sos_filter(&x[start_index], &y[start_index], &sos_filt, DATA_CHUNK_SIZE);
	}
	/* Print out the data to file */
	FILE *in_file = fopen(in_filename, "w");

	fprintf(in_file, "Time,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++)
	{
		fprintf(in_file, "%f,%f\n", t[i], x[i]);
	}

	fclose(in_file);

	FILE *out_file = fopen(out_filename, "w");
	fprintf(out_file, "Time,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++)
	{
		fprintf(out_file, "%f,%f\n", t[i], y[i]);
	}

	fclose(out_file);

	return 0;
}

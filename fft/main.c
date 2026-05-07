#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "fft.h"
#include "../inc/complex_float.h"
#include "../inc/waveform_gen_float.h"

/* Define the frequency at which the waveform is sampled */
#define SAMPLING_FREQUENCY 512

/* Define the length of the data to work on */
/* For optimal performance choose a data size that is a power of 2 */
#define DATA_SIZE 8192

/* Define the parameters of the input waveform */
wave_properties_float_t wave_1[] = {
	[0] = {
		.amplitude = 10,
		.offset = 0,
		.frequency = 10,
		.phase = 0,
		.function_type = FUNCTION_COS
		},
	[1] = {
		.amplitude = 50,
		.offset = 0,
		.frequency = 50,
		.phase = 0,
		.function_type = FUNCTION_COS
		},
};

wave_settings_complex_float_t wave_1_settings = {
	.fs = SAMPLING_FREQUENCY,
	.data_length = DATA_SIZE,
	.dc_offset.real = 0,
	.dc_offset.imag = 0,
	.superposition_real = wave_1, .superposition_imag = NULL,
	.num_superpositions = sizeof(wave_1) / sizeof(wave_1[0]),
};

const char in_filename[] = "input_data.csv";
const char out_filename[] = "output_data.csv";

int main()
{
	float t[DATA_SIZE];
	complex_float_t y[DATA_SIZE];
	float f[DATA_SIZE / 2];
	complex_float_t y_fft[DATA_SIZE];

	/* Initialise the arrays to zero */
	for (uint32_t i = 0; i < DATA_SIZE; i++)
	{
		y[i].real = 0;
		y[i].imag = 0;
		y_fft[i].real = 0;
		y_fft[i].imag = 0;
	}

	/* Generate time and data */
	/* Only using waveform with non-zero real component for now */
	init_waveform_complex_float(wave_1_settings, t, y);

	/* Print out the input data to file */
	FILE* in_file = fopen(in_filename, "w");

	fprintf(in_file, "Time,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++)
	{
		fprintf(in_file, "%f,%f\n", t[i], y[i].real);
	}

	fclose(in_file);


	/* Perform the transform */
	ft(y, y_fft, DATA_SIZE);
	//fft(y, y_fft, DATA_SIZE);

	/* Generate frequencies to match the fft against */
	gen_freq_float(f, DATA_SIZE / 2, SAMPLING_FREQUENCY);


	/* Print out the output data to file */
	FILE* out_file = fopen(out_filename, "w");
	fprintf(out_file, "t,y_real,y_imag, amplitude\n");
	for (int i = 0; i < DATA_SIZE / 2; i++)
	{
		float amplitude = (2 * complex_magnitude_float(y_fft[i])) / DATA_SIZE;
		fprintf(out_file, "%f,%f,%f,%f\n", f[i], y_fft[i].real, y_fft[i].imag, amplitude);
	}

	fclose(out_file);

	return 0;
}

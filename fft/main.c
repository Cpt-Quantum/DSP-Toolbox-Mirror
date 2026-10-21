#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/complex_float.h"
#include "../inc/waveform_gen_float.h"
#include "fft_float.h"

/* Define the filenames for the input data file and the output data files */
const char in_filename[] = "results/input_data.csv";
const char fft_filename[] = "results/fft_data.csv";
const char ifft_filename[] = "results/ifft_data.csv";

/* Define the frequency at which the waveform is sampled */
#define SAMPLE_RATE 512

/* Define the length of the data to transform, this must be a power of 2 */
#define DATA_SIZE 8192

/* Define the parameters of the input waveform, a pair of tones */
wave_properties_float_t input_waves[] = {
	{.amplitude = 10,
	 .offset = 0,
	 .frequency = 10,
	 .phase = 0,
	 .function_type = FUNCTION_COS},
	{.amplitude = 50,
	 .offset = 0,
	 .frequency = 50,
	 .phase = 0,
	 .function_type = FUNCTION_COS},
};

void fprint_data(const char *filename, const char *x_label, float *x,
				 complex_float_t *y, uint32_t data_length) {
	/* Open the file and check it opened successfully, if not then exit */
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		printf("Failed to open file %s, exiting\n", filename);
		exit(EXIT_FAILURE);
	}
	/* Print the header */
	fprintf(file, "%s,Real,Imag\n", x_label);
	/* Print the x axis and complex data to file */
	for (uint32_t i = 0; i < data_length; i++) {
		fprintf(file, "%.9g,%.9g,%.9g\n", x[i], y[i].real, y[i].imag);
	}
	/* Close the file */
	fclose(file);
}

int main() {
	/* Create the time, frequency and data arrays */
	float *t = malloc(DATA_SIZE * sizeof(float));
	float *f = malloc(DATA_SIZE * sizeof(float));
	complex_float_t *data_in = calloc(DATA_SIZE, sizeof(complex_float_t));
	complex_float_t *fft_data = malloc(DATA_SIZE * sizeof(complex_float_t));
	complex_float_t *ifft_data = malloc(DATA_SIZE * sizeof(complex_float_t));

	/* Create the FFT plan, its twiddle factors are stored in a caller provided
	 * workspace */
	struct fft_plan_float_t plan;
	size_t work_bytes = fft_plan_size_float(DATA_SIZE);
	void *work = malloc(work_bytes);
	FFT_STATUS_E status =
		fft_plan_init_float(&plan, DATA_SIZE, work, work_bytes);
	if (status != FFT_OK) {
		printf("Failed to create the FFT plan (status %d), exiting\n", status);
		exit(EXIT_FAILURE);
	}

	/* Generate the input waveform, only the real part is used */
	printf("Generating test waveform.\n");
	wave_settings_complex_float_t wave_settings = {
		.fs = SAMPLE_RATE,
		.data_length = DATA_SIZE,
		.superposition_real = input_waves,
		.superposition_imag = NULL,
		.num_superpositions = sizeof(input_waves) / sizeof(input_waves[0]),
	};
	init_waveform_complex_float(wave_settings, t, data_in);

	/* The frequency of each bin, bins above DATA_SIZE / 2 are the negative
	 * frequencies */
	for (uint32_t i = 0; i < DATA_SIZE; i++) {
		f[i] = (float)i * SAMPLE_RATE / DATA_SIZE;
	}

	/* Perform the transforms. They work in place, so the input is copied
	 * first to keep it for comparison */
	printf("Performing the FFT and the inverse FFT.\n");
	memcpy(fft_data, data_in, DATA_SIZE * sizeof(complex_float_t));
	fft_float(&plan, fft_data);
	memcpy(ifft_data, fft_data, DATA_SIZE * sizeof(complex_float_t));
	ifft_float(&plan, ifft_data);
	printf("Transforms completed. Outputting all data to files.\n");

	/* Print all data to file, including the data input */
	fprint_data(in_filename, "Time", t, data_in, DATA_SIZE);
	fprint_data(fft_filename, "Frequency", f, fft_data, DATA_SIZE);
	fprint_data(ifft_filename, "Time", t, ifft_data, DATA_SIZE);

	/* Free the memory reserved for the data and the plan */
	free(t);
	free(f);
	free(data_in);
	free(fft_data);
	free(ifft_data);
	free(work);

	return 0;
}

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/spectrum_gen_float.h"
#include "fir_float.h"
#include "fir_int32.h"
#include "iir_float.h"
#include "iir_int32.h"
#include "main_inc.h"

const char filter_json_filename[] = "results/filters.json";
/* Define the filenames for the input data file and the output data file */
const char in_filename[] = "results/input_data.csv";
const char iir_simple_filename[] = "results/iir_simple_data.csv";
const char iir_sos_filename[] = "results/iir_sos_data.csv";
const char fir_filename[] = "results/fir_data.csv";
const char fir_decimation_filename[] = "results/fir_decimation_data.csv";
const char iir_simple_int32_filename[] = "results/iir_simple_int32_data.csv";
const char iir_sos_int32_filename[] = "results/iir_sos_int32_data.csv";
const char fir_int32_filename[] = "results/fir_int32_data.csv";
const char fir_decimation_int32_filename[] =
	"results/fir_decimation_int32_data.csv";

/* Define any filter constants not set in the main_inc file */
#define DECIMATION_RATE 4
#define SPECTRUM_FREQUENCY_BINS 1000

/* Define the length of the data to work on. These aren't strictly necessary,
 * the filters can be ran against a full long dataset. This just tests filter
 * behaviour in a real time processing environment. */
#define DATA_CHUNK_SIZE 16
#define DATA_CHUNK_COUNT (f_s)
#define DATA_SIZE (DATA_CHUNK_SIZE * DATA_CHUNK_COUNT)

void fprint_data(const char *filename, float *t, float *x,
				 unsigned int data_length, uint8_t decimation_rate) {
	/* Open the file and check it opened successfully, if not then exit */
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		printf("Failed to open file %s, exiting\n", filename);
		exit(EXIT_FAILURE);
	}
	/* Print the header */
	fprintf(file, "Time,Amplitude\n");
	/* Print the time data and waveform data to file */
	if (decimation_rate > 1) {
		/* Only print every Nth time value in the case of decimated data, where
		 * N = decimation_rate */
		for (int i = 0; i < data_length; i++) {
			fprintf(file, "%.9g,%.9g\n", t[decimation_rate * i], x[i]);
		}
	} else {
		/* No decimation on the data, so use all time values */
		for (int i = 0; i < data_length; i++) {
			fprintf(file, "%.9g,%.9g\n", t[i], x[i]);
		}
	}
	/* Close the file */
	fclose(file);
}

/* Converts the float input to int32 samples for the integer filters. The scale
 * is the largest power of two that keeps the peak within half of full scale,
 * leaving headroom for the filters' passband ripple. As a power of two, the
 * int32 samples hold the float samples exactly. Returns the scale used */
float float_to_int32(const float *x, int32_t *y, uint32_t length) {
	float peak = 0.0f;
	for (uint32_t i = 0; i < length; i++) {
		peak = fabsf(x[i]) > peak ? fabsf(x[i]) : peak;
	}
	float scale = 1.0f;
	while (peak > 0.0f && peak * scale * 2.0f <= (float)(1 << 30)) {
		scale *= 2.0f;
	}
	for (uint32_t i = 0; i < length; i++) {
		y[i] = (int32_t)lrintf(x[i] * scale);
	}
	return scale;
}

/* Converts int32 samples back to the float range of the input, for output */
void int32_to_float(const int32_t *x, float *y, uint32_t length, float scale) {
	for (uint32_t i = 0; i < length; i++) {
		y[i] = (float)x[i] / scale;
	}
}

int main() {
	/* Initialise the filters */
	filter_init(filter_json_filename);

	/* Create the data and time arrays */
	float *t = malloc(DATA_SIZE * sizeof(float));
	float *data_in = malloc(DATA_SIZE * sizeof(float));
	float *iir_simple_filtered_data = malloc(DATA_SIZE * sizeof(float));
	float *iir_sos_filtered_data = malloc(DATA_SIZE * sizeof(float));
	float *fir_filtered_data = malloc(DATA_SIZE * sizeof(float));
	float *fir_decimation_filtered_data =
		malloc((DATA_SIZE / DECIMATION_RATE) * sizeof(float));
	/* The integer copies of the input and filtered data */
	int32_t *data_in_int32 = malloc(DATA_SIZE * sizeof(int32_t));
	int32_t *iir_simple_filtered_int32 = malloc(DATA_SIZE * sizeof(int32_t));
	int32_t *iir_sos_filtered_int32 = malloc(DATA_SIZE * sizeof(int32_t));
	int32_t *fir_filtered_int32 = malloc(DATA_SIZE * sizeof(int32_t));
	int32_t *fir_decimation_filtered_int32 =
		malloc((DATA_SIZE / DECIMATION_RATE) * sizeof(int32_t));
	/* Buffer to convert the integer outputs back to floats for printing */
	float *int32_print_data = malloc(DATA_SIZE * sizeof(float));

	/* Generate a spectrum for the input waveform */
	printf("Generating test waveform.\n");
	generate_spectrum_waveform_float(t, data_in, DATA_SIZE, f_s,
									 SPECTRUM_FREQUENCY_BINS,
									 AMPLITUDE_SPECTRUM_FLAT, 1.0f, 0.0f, 0);
	const float int32_scale = float_to_int32(data_in, data_in_int32, DATA_SIZE);
	printf("Scaled the input by %g for the int32 filters.\n", int32_scale);

	/* Perform the filter */
	printf("Performing filters against the input data.\n");
	/* The data is sent through in chunks to simulate live data */
	uint32_t start_index = 0;
	for (uint32_t i = 0; i < DATA_CHUNK_COUNT; i++) {
		/* Call each filter type */
		iir_filter_float(&data_in[start_index],
						 &iir_simple_filtered_data[start_index], &iir_filt,
						 DATA_CHUNK_SIZE);
		sos_filter_float(&data_in[start_index],
						 &iir_sos_filtered_data[start_index], &sos_filt,
						 DATA_CHUNK_SIZE);
		fir_filter_float(&data_in[start_index], &fir_filtered_data[start_index],
						 &fir_filt, DATA_CHUNK_SIZE);
		fir_decimate_float(
			&data_in[start_index],
			&fir_decimation_filtered_data[start_index / DECIMATION_RATE],
			&fir_decimation_filt, DATA_CHUNK_SIZE, DECIMATION_RATE);
		/* And the same filters with Q format integer coefficients */
		iir_filter_int32(&data_in_int32[start_index],
						 &iir_simple_filtered_int32[start_index],
						 &iir_filt_int32, DATA_CHUNK_SIZE);
		sos_filter_int32(&data_in_int32[start_index],
						 &iir_sos_filtered_int32[start_index], &sos_filt_int32,
						 DATA_CHUNK_SIZE);
		fir_filter_int32(&data_in_int32[start_index],
						 &fir_filtered_int32[start_index], &fir_filt_int32,
						 DATA_CHUNK_SIZE);
		fir_decimate_int32(
			&data_in_int32[start_index],
			&fir_decimation_filtered_int32[start_index / DECIMATION_RATE],
			&fir_decimation_filt_int32, DATA_CHUNK_SIZE, DECIMATION_RATE);

		/* Increment the start index */
		start_index += DATA_CHUNK_SIZE;
	}
	printf("Filtering of data completed. Outputting all data to files.\n");

	/* Print all data to file, including the data input */
	fprint_data(in_filename, t, data_in, DATA_SIZE, 1);
	fprint_data(iir_simple_filename, t, iir_simple_filtered_data, DATA_SIZE, 1);
	fprint_data(iir_sos_filename, t, iir_sos_filtered_data, DATA_SIZE, 1);
	fprint_data(fir_filename, t, fir_filtered_data, DATA_SIZE, 1);
	fprint_data(fir_decimation_filename, t, fir_decimation_filtered_data,
				DATA_SIZE / DECIMATION_RATE, DECIMATION_RATE);
	/* The integer outputs are converted back to the input's range first */
	int32_to_float(iir_simple_filtered_int32, int32_print_data, DATA_SIZE,
				   int32_scale);
	fprint_data(iir_simple_int32_filename, t, int32_print_data, DATA_SIZE, 1);
	int32_to_float(iir_sos_filtered_int32, int32_print_data, DATA_SIZE,
				   int32_scale);
	fprint_data(iir_sos_int32_filename, t, int32_print_data, DATA_SIZE, 1);
	int32_to_float(fir_filtered_int32, int32_print_data, DATA_SIZE,
				   int32_scale);
	fprint_data(fir_int32_filename, t, int32_print_data, DATA_SIZE, 1);
	int32_to_float(fir_decimation_filtered_int32, int32_print_data,
				   DATA_SIZE / DECIMATION_RATE, int32_scale);
	fprint_data(fir_decimation_int32_filename, t, int32_print_data,
				DATA_SIZE / DECIMATION_RATE, DECIMATION_RATE);

	/* Free the memory reserved for the data */
	free(t);
	free(data_in);
	free(iir_simple_filtered_data);
	free(iir_sos_filtered_data);
	free(fir_filtered_data);
	free(fir_decimation_filtered_data);
	free(data_in_int32);
	free(iir_simple_filtered_int32);
	free(iir_sos_filtered_int32);
	free(fir_filtered_int32);
	free(fir_decimation_filtered_int32);
	free(int32_print_data);
	/* Deinit the filters */
	filter_deinit();

	return 0;
}

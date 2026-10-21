#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/waveform_gen_float.h"
#include "avg_float.h"
#include "avg_int32.h"

/* Define the filenames for the input data file and the output data files */
const char in_filename[] = "results/input_data.csv";
const char detrend_filename[] = "results/detrend_data.csv";
const char power_filename[] = "results/power_data.csv";

/* Define the frequency at which the waveform is sampled, and its length */
#define SAMPLE_RATE 40000
#define DATA_SIZE (SAMPLE_RATE * 2)

/* The input is a set of tones on a DC offset, as from an ADC centred on half
 * of a 3.3V supply */
#define DC_OFFSET (3.3f / 2)
static wave_properties_float_t input_waves[] = {
	{.amplitude = 1, .frequency = 80, .function_type = FUNCTION_SINE},
	{.amplitude = 0.8f, .frequency = 200, .function_type = FUNCTION_SINE},
	{.amplitude = 1.2f, .frequency = 1000, .function_type = FUNCTION_SINE},
	{.amplitude = 0.9f, .frequency = 4000, .function_type = FUNCTION_SINE},
	{.amplitude = 0.5f, .frequency = 8000, .function_type = FUNCTION_SINE},
};

/* The int32 copy of the input is in fixed point with 20 fractional bits, like
 * the counts from an ADC */
#define INT32_SCALE (1 << 20)

/* The DC offset is removed in two ways. The first subtracts the average of
 * each block of DETREND_LENGTH samples. The second averages chunks of
 * CHUNK_LENGTH samples, then subtracts a moving average of the chunk averages
 * covering the same DETREND_LENGTH samples, which follows a changing offset
 * without steps between blocks */
#define DETREND_LENGTH (SAMPLE_RATE / 10)
#define CHUNK_LENGTH 50
#define MOVING_AVG_LENGTH (DETREND_LENGTH / CHUNK_LENGTH)

/* The power of the detrended signal is calculated over every POWER_LENGTH
 * samples (1ms) */
#define POWER_LENGTH (SAMPLE_RATE / 1000)
#define POWER_SIZE (DATA_SIZE / POWER_LENGTH)

void fprint_data(const char *filename, const char *header, const float *t,
				 uint32_t t_stride, const float *const *columns,
				 uint32_t num_columns, uint32_t data_length) {
	/* Open the file and check it opened successfully, if not then exit */
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		printf("Failed to open file %s, exiting\n", filename);
		exit(EXIT_FAILURE);
	}
	/* Print the header */
	fprintf(file, "%s\n", header);
	/* Print the time of each row followed by each column's data */
	for (uint32_t i = 0; i < data_length; i++) {
		fprintf(file, "%.9g", t[i * t_stride]);
		for (uint32_t c = 0; c < num_columns; c++) {
			fprintf(file, ",%.9g", columns[c][i]);
		}
		fprintf(file, "\n");
	}
	/* Close the file */
	fclose(file);
}

/* Removes the DC offset of x with both methods, then calculates the power of
 * the block detrended signal */
void detrend_float(const float *x, float *block, float *moving, float *power) {
	for (uint32_t i = 0; i < DATA_SIZE; i += DETREND_LENGTH) {
		const float avg = average_float(&x[i], DETREND_LENGTH);
		for (uint32_t j = i; j < i + DETREND_LENGTH; j++) {
			block[j] = x[j] - avg;
		}
	}

	float window[MOVING_AVG_LENGTH];
	struct moving_avg_float_t moving_avg;
	moving_avg_init_float(&moving_avg, window, MOVING_AVG_LENGTH);
	for (uint32_t i = 0; i < DATA_SIZE; i += CHUNK_LENGTH) {
		const float avg = moving_average_float(
			&moving_avg, average_float(&x[i], CHUNK_LENGTH));
		for (uint32_t j = i; j < i + CHUNK_LENGTH; j++) {
			moving[j] = x[j] - avg;
		}
	}

	for (uint32_t i = 0; i < POWER_SIZE; i++) {
		power[i] = power_average_float(&block[i * POWER_LENGTH], POWER_LENGTH);
	}
}

/* The same as detrend_float() for int32 samples. The power is returned as
 * int64_t, as the squares don't fit in int32_t */
void detrend_int32(const int32_t *x, int32_t *block, int32_t *moving,
				   int64_t *power) {
	for (uint32_t i = 0; i < DATA_SIZE; i += DETREND_LENGTH) {
		const int32_t avg = average_int32(&x[i], DETREND_LENGTH);
		for (uint32_t j = i; j < i + DETREND_LENGTH; j++) {
			block[j] = x[j] - avg;
		}
	}

	int32_t window[MOVING_AVG_LENGTH];
	struct moving_avg_int32_t moving_avg;
	moving_avg_init_int32(&moving_avg, window, MOVING_AVG_LENGTH);
	for (uint32_t i = 0; i < DATA_SIZE; i += CHUNK_LENGTH) {
		const int32_t avg = moving_average_int32(
			&moving_avg, average_int32(&x[i], CHUNK_LENGTH));
		for (uint32_t j = i; j < i + CHUNK_LENGTH; j++) {
			moving[j] = x[j] - avg;
		}
	}

	for (uint32_t i = 0; i < POWER_SIZE; i++) {
		power[i] = power_average_int32(&block[i * POWER_LENGTH], POWER_LENGTH);
	}
}

int main() {
	/* Create the time and data arrays */
	float *t = malloc(DATA_SIZE * sizeof(float));
	/* Zeroed, as the waveform generation adds each wave into the buffer */
	float *data_in = calloc(DATA_SIZE, sizeof(float));
	float *block = malloc(DATA_SIZE * sizeof(float));
	float *moving = malloc(DATA_SIZE * sizeof(float));
	float *power = malloc(POWER_SIZE * sizeof(float));
	int32_t *data_in_int32 = malloc(DATA_SIZE * sizeof(int32_t));
	int32_t *block_int32 = malloc(DATA_SIZE * sizeof(int32_t));
	int32_t *moving_int32 = malloc(DATA_SIZE * sizeof(int32_t));
	int64_t *power_int32 = malloc(POWER_SIZE * sizeof(int64_t));
	/* Buffers to convert the integer outputs back to floats for printing */
	float *block_int32_print = malloc(DATA_SIZE * sizeof(float));
	float *moving_int32_print = malloc(DATA_SIZE * sizeof(float));
	float *power_int32_print = malloc(POWER_SIZE * sizeof(float));

	/* Generate the input waveform, and its int32 copy */
	printf("Generating test waveform.\n");
	wave_settings_float_t wave_settings = {
		.fs = SAMPLE_RATE,
		.data_length = DATA_SIZE,
		.dc_offset = DC_OFFSET,
		.superposition = input_waves,
		.num_superpositions = sizeof(input_waves) / sizeof(input_waves[0]),
	};
	init_waveform_float(wave_settings, t, data_in);
	for (uint32_t i = 0; i < DATA_SIZE; i++) {
		data_in_int32[i] = (int32_t)lrintf(data_in[i] * INT32_SCALE);
	}

	/* Remove the DC offset and calculate the power */
	printf("Removing the DC offset and calculating the power.\n");
	detrend_float(data_in, block, moving, power);
	detrend_int32(data_in_int32, block_int32, moving_int32, power_int32);
	printf("Averaging completed. Outputting all data to files.\n");

	/* The integer outputs are converted back to the input's range first */
	for (uint32_t i = 0; i < DATA_SIZE; i++) {
		block_int32_print[i] = (float)block_int32[i] / INT32_SCALE;
		moving_int32_print[i] = (float)moving_int32[i] / INT32_SCALE;
	}
	for (uint32_t i = 0; i < POWER_SIZE; i++) {
		power_int32_print[i] = (float)((double)power_int32[i] /
									   ((double)INT32_SCALE * INT32_SCALE));
	}

	/* Print all data to file, including the data input */
	const float *in_columns[] = {data_in};
	fprint_data(in_filename, "Time,Amplitude", t, 1, in_columns, 1, DATA_SIZE);
	const float *detrend_columns[] = {block, moving, block_int32_print,
									  moving_int32_print};
	fprint_data(detrend_filename, "Time,Block,Moving,Block_int32,Moving_int32",
				t, 1, detrend_columns, 4, DATA_SIZE);
	const float *power_columns[] = {power, power_int32_print};
	fprint_data(power_filename, "Time,Power,Power_int32", t, POWER_LENGTH,
				power_columns, 2, POWER_SIZE);

	/* Free the memory reserved for the data */
	free(t);
	free(data_in);
	free(block);
	free(moving);
	free(power);
	free(data_in_int32);
	free(block_int32);
	free(moving_int32);
	free(power_int32);
	free(block_int32_print);
	free(moving_int32_print);
	free(power_int32_print);

	return 0;
}

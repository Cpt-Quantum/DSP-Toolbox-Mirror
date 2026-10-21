#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/waveform_gen_float.h"

/* Define the filename for the output data file */
const char out_filename[] = "results/waveform_data.csv";

/* Define the frequency at which the waveforms are sampled, and their length
 * (1 second) */
#define SAMPLE_RATE 2000
#define DATA_SIZE SAMPLE_RATE

/* One wave of each of the non sinusoidal functions. The members not used by a
 * function are left as 0 */
static wave_properties_float_t waves[] = {
	{.amplitude = 1,
	 .frequency = 7,
	 .function_type = FUNCTION_SQUARE,
	 .duty_cycle = 0.3f},
	{.amplitude = 1, .frequency = 7, .function_type = FUNCTION_TRIANGLE},
	{.amplitude = 1,
	 .frequency = 7,
	 .phase = 0.25f,
	 .function_type = FUNCTION_SAWTOOTH},
	/* Sweeps from 10 Hz up to 100 Hz over the second */
	{.amplitude = 1,
	 .frequency = 10,
	 .function_type = FUNCTION_CHIRP,
	 .sweep_rate = 90},
	{.amplitude = 1, .function_type = FUNCTION_IMPULSE, .delay = 0.25f},
	{.amplitude = 1, .function_type = FUNCTION_STEP, .delay = 0.5f},
};
static const char *wave_names[] = {"Square",  "Triangle", "Sawtooth",
								   "Chirp",   "Impulse",  "Step"};
#define NUM_WAVES (sizeof(waves) / sizeof(waves[0]))

/* A superposition of several functions, as a test signal: a step up to 1 at
 * 0.2s, an impulse of 2 at 0.6s and a small square wave riding on a sine */
static wave_properties_float_t composite_waves[] = {
	{.amplitude = 1, .function_type = FUNCTION_STEP, .delay = 0.2f},
	{.amplitude = 2, .function_type = FUNCTION_IMPULSE, .delay = 0.6f},
	{.amplitude = 0.2f, .frequency = 23, .function_type = FUNCTION_SQUARE},
	{.amplitude = 0.5f, .frequency = 3, .function_type = FUNCTION_SINE},
};

int main() {
	/* Create the time array, and a zeroed data array for each wave plus the
	 * composite, as the waves are added into the buffers */
	float *t = malloc(DATA_SIZE * sizeof(float));
	float *data[NUM_WAVES + 1];
	for (uint32_t w = 0; w <= NUM_WAVES; w++) {
		data[w] = calloc(DATA_SIZE, sizeof(float));
	}

	/* Generate each wave on its own */
	printf("Generating test waveforms.\n");
	gen_time_float(t, DATA_SIZE, SAMPLE_RATE);
	for (uint32_t w = 0; w < NUM_WAVES; w++) {
		waveform_add_superposition_float(waves[w], t, data[w], DATA_SIZE);
	}

	/* Generate the composite from its settings */
	wave_settings_float_t composite_settings = {
		.fs = SAMPLE_RATE,
		.data_length = DATA_SIZE,
		.dc_offset = 0,
		.superposition = composite_waves,
		.num_superpositions =
			sizeof(composite_waves) / sizeof(composite_waves[0]),
	};
	init_waveform_float(composite_settings, t, data[NUM_WAVES]);
	printf("Waveforms generated. Outputting all data to file.\n");

	/* Open the file and check it opened successfully, if not then exit */
	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Failed to open file %s, exiting\n", out_filename);
		exit(EXIT_FAILURE);
	}
	/* Print the header, then the time and each wave per row */
	fprintf(file, "Time");
	for (uint32_t w = 0; w < NUM_WAVES; w++) {
		fprintf(file, ",%s", wave_names[w]);
	}
	fprintf(file, ",Composite\n");
	for (uint32_t i = 0; i < DATA_SIZE; i++) {
		fprintf(file, "%.9g", t[i]);
		for (uint32_t w = 0; w <= NUM_WAVES; w++) {
			fprintf(file, ",%.9g", data[w][i]);
		}
		fprintf(file, "\n");
	}
	fclose(file);

	/* Free the memory reserved for the data */
	free(t);
	for (uint32_t w = 0; w <= NUM_WAVES; w++) {
		free(data[w]);
	}

	return 0;
}

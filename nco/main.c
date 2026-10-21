#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/nco_float.h"
#include "../inc/waveform_gen_float.h"

/* Define the filename for the output data file */
const char out_filename[] = "results/nco_data.csv";

/* The waveforms are streamed in blocks of BLOCK_SIZE samples, as they would
 * be to a DAC or from an interrupt, for 1 second */
#define SAMPLE_RATE 2000
#define DATA_SIZE SAMPLE_RATE
#define BLOCK_SIZE 50

/* The same waves as the waveform example, plus a sine that hops frequency */
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
	/* Hops from 7 Hz to 15 Hz half way through, see HOP_SAMPLE */
	{.amplitude = 1, .frequency = 7, .function_type = FUNCTION_SINE},
};
static const char *wave_names[] = {"Square", "Triangle", "Sawtooth", "Chirp",
								   "Hop"};
#define NUM_WAVES (sizeof(waves) / sizeof(waves[0]))
#define HOP_WAVE 4
#define HOP_SAMPLE (DATA_SIZE / 2)
#define HOP_FREQUENCY 15.0f

/* A complex oscillator at -5 Hz, e^(-j 2 pi 5 t), as used to mix a signal
 * down in frequency */
static wave_properties_float_t mixer = {
	.amplitude = 1, .frequency = -5, .function_type = FUNCTION_COS};

int main() {
	float *t = malloc(DATA_SIZE * sizeof(float));
	float *block_data = malloc(DATA_SIZE * sizeof(float));
	float *data[NUM_WAVES];
	nco_float_t oscillators[NUM_WAVES];
	for (uint32_t w = 0; w < NUM_WAVES; w++) {
		data[w] = malloc(DATA_SIZE * sizeof(float));
		if (nco_init_float(&oscillators[w], waves[w], SAMPLE_RATE) != NCO_OK) {
			printf("Failed to initialise the %s oscillator\n", wave_names[w]);
			exit(EXIT_FAILURE);
		}
	}
	complex_float_t *mixer_data = malloc(DATA_SIZE * sizeof(complex_float_t));
	nco_float_t mixer_nco;
	nco_init_float(&mixer_nco, mixer, SAMPLE_RATE);

	/* Stream each oscillator a block at a time. Each call carries on from
	 * where the last one left off */
	printf("Streaming the waveforms in blocks of %d samples.\n", BLOCK_SIZE);
	for (uint32_t start = 0; start < DATA_SIZE; start += BLOCK_SIZE) {
		if (start == HOP_SAMPLE) {
			/* The phase carries on smoothly through the change */
			nco_set_frequency_float(&oscillators[HOP_WAVE], HOP_FREQUENCY);
		}
		for (uint32_t w = 0; w < NUM_WAVES; w++) {
			nco_generate_float(&oscillators[w], data[w] + start, BLOCK_SIZE);
		}
		nco_generate_complex_float(&mixer_nco, mixer_data + start, BLOCK_SIZE);
	}

	/* The streamed waves match the block generator, which needs the whole
	 * time buffer at once. The hop isn't compared, as the block generator
	 * can't change frequency */
	gen_time_float(t, DATA_SIZE, SAMPLE_RATE);
	for (uint32_t w = 0; w < NUM_WAVES; w++) {
		if (w == HOP_WAVE) {
			continue;
		}
		for (uint32_t i = 0; i < DATA_SIZE; i++) {
			block_data[i] = 0;
		}
		waveform_add_superposition_float(waves[w], t, block_data, DATA_SIZE);
		float max_difference = 0;
		uint32_t edges = 0;
		for (uint32_t i = 0; i < DATA_SIZE; i++) {
			const float difference = fabsf(data[w][i] - block_data[i]);
			/* The square and sawtooth can be either side of an edge that
			 * falls on a sample, from rounding in the block generator */
			if (difference > 1.0f) {
				edges++;
			} else if (difference > max_difference) {
				max_difference = difference;
			}
		}
		printf("%-9s max difference to waveform_add_superposition_float() "
			   "%.3g (%u sample(s) on an edge)\n",
			   wave_names[w], max_difference, edges);
	}
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
	fprintf(file, ",Mixer real,Mixer imag\n");
	for (uint32_t i = 0; i < DATA_SIZE; i++) {
		fprintf(file, "%.9g", t[i]);
		for (uint32_t w = 0; w < NUM_WAVES; w++) {
			fprintf(file, ",%.9g", data[w][i]);
		}
		fprintf(file, ",%.9g,%.9g\n", mixer_data[i].real, mixer_data[i].imag);
	}
	fclose(file);

	/* Free the memory reserved for the data */
	free(t);
	free(block_data);
	free(mixer_data);
	for (uint32_t w = 0; w < NUM_WAVES; w++) {
		free(data[w]);
	}

	return 0;
}

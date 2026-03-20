#include "../inc/waveform_gen_float.h"
#include "cpp_wrapper.h"
#include <cstdio>
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

#define SAMPLE_RATE_HZ 16000
#define N_SAMPLES 32000

typedef enum { AMPLITUDE_SHAPE_FLAT, AMPLITUDE_SHAPE_TAIL } wave_shape_t;

vector<wave_properties_float_t>
wave_properties_gen(float sample_rate, unsigned int n_waves, float amplitude,
					wave_shape_t amplitude_shape, float dc_offset) {
	vector<wave_properties_float_t> waves;
	wave_properties_float_t wave_tmp;
	float freq_step = sample_rate / n_waves;

	/* Set the DC offset first */
	wave_tmp.amplitude = 0;
	wave_tmp.offset = dc_offset;
	/* Just initialise the other properties to default values */
	wave_tmp.frequency = 0;
	wave_tmp.phase = 0;
	wave_tmp.function_type = FUNCTION_SINE_FLOAT;
	waves.push_back(wave_tmp);

	/* Reset the offset to 0 as it doesn't need to be applied now */
	wave_tmp.offset = 0;

	for (unsigned int i = 0; i < n_waves; i++) {
		/* Generate the amplitudes based on wave shape */
		switch (amplitude_shape) {
		case AMPLITUDE_SHAPE_FLAT:
			wave_tmp.amplitude = amplitude;
			break;
		case AMPLITUDE_SHAPE_TAIL:
			if (i == n_waves / 2) {
				wave_tmp.amplitude = amplitude;
			} else if (i < n_waves / 2) {
				wave_tmp.amplitude =
					amplitude * expf((i - (float)n_waves / 2) / n_waves);
			} else if (i > n_waves / 2) {
				wave_tmp.amplitude =
					amplitude * expf(((float)n_waves / 2 - i) / n_waves);
			}
			break;
		default:
			break;
		}
		/* Set the rest of the wave properties before adding the struct to the
		 * vector */
		wave_tmp.frequency = (i + 1) * freq_step;
		waves.push_back(wave_tmp);
	}

	return waves;
}
void wave_fprintf(const char *filename, waveform_t t, waveform_x x) {
	/* Open the file */
	FILE *out_file = fopen(filename, "w");
	if (out_file == NULL) {
		printf("File with filename: %s failed to open", filename);
	}
	/* Print the header information */
	fprintf(out_file, "Time(s),Amplitude(V)\n");
	/* Now print all the time and waveform data */
	for (unsigned int i = 0; i < t.t.size(); i++) {
		fprintf(out_file, "%f,%f\n", t.t.data()[i], x.x.data()[i]);
	}
	/* Close the file */
	fclose(out_file);
}

int main(void) {
	/* Generate 2 sets of waveform properties */
	vector<wave_properties_float_t> waves_flat = wave_properties_gen(
		SAMPLE_RATE_HZ, SAMPLE_RATE_HZ / 100, 100, AMPLITUDE_SHAPE_FLAT, 0);
	vector<wave_properties_float_t> waves_tail = wave_properties_gen(
		SAMPLE_RATE_HZ, SAMPLE_RATE_HZ / 100, 100, AMPLITUDE_SHAPE_TAIL, 0);
	printf("Wave properties vector sizes:\n");
	printf("Flat: %lu, Tail: %lu\n", waves_flat.size(), waves_tail.size());
	/* Initialise the time series data */
	waveform_t time_series(SAMPLE_RATE_HZ, N_SAMPLES);
	printf("Time series length: %lu\n", time_series.t.size());
	/* Now initialise the waveforms */
	waveform_x x_flat(N_SAMPLES, waves_flat);
	x_flat.gen_waveform(time_series.t.data());
	waveform_x x_tail(N_SAMPLES, waves_tail);
	x_tail.gen_waveform(time_series.t.data());

	printf("Flat wave length: %lu, tail wave length: %lu\n", x_flat.x.size(),
		   x_tail.x.size());
	/* Print both waveforms to file */
	wave_fprintf("flat_data.csv", time_series, x_flat);
	wave_fprintf("tail_data.csv", time_series, x_tail);
}

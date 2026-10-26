#include "../../cpp_wrapper_average.h"
#include "../../cpp_wrapper_waveform.h"
#include <cmath>
#include <cstdio>

using namespace std;

#define SAMPLE_RATE 40000
#define DATA_LENGTH (SAMPLE_RATE / 2)
#define NUM_CHANNELS 4
/* The moving average spans 10ms, a whole number of periods of the tone */
#define WINDOW_LENGTH (SAMPLE_RATE / 100)
/* The int32 samples are in fixed point with 20 fractional bits, like the
 * counts from an ADC */
#define INT32_SCALE (1 << 20)

int main() {
	waveform_t<float> t(SAMPLE_RATE, DATA_LENGTH);

	/* Each channel has the same tone on a different DC offset, and an int32
	 * copy of it */
	vector<waveform_x<float>> ch_data;
	vector<vector<int32_t>> ch_data_int32;
	for (int i = 0; i < NUM_CHANNELS; i++) {
		ch_data.push_back(waveform_x<float>(DATA_LENGTH));
		ch_data[i].add_wave_properties(1.0f, 0.5f * (float)i, 200.0f, 0.0f,
									   FUNCTION_SINE);
		ch_data[i].gen_waveform(t.t);

		vector<int32_t> x(DATA_LENGTH);
		for (unsigned int j = 0; j < DATA_LENGTH; j++) {
			x[j] = (int32_t)lrintf(ch_data[i].x[j] * INT32_SCALE);
		}
		ch_data_int32.push_back(x);
	}

	/* One reference moving average, copied for each channel so they each have
	 * their own window */
	moving_avg<float> ref_avg(WINDOW_LENGTH);
	moving_avg<int32_t> ref_avg_int32(WINDOW_LENGTH);
	vector<moving_avg<float>> ch_avg(NUM_CHANNELS, ref_avg);
	vector<moving_avg<int32_t>> ch_avg_int32(NUM_CHANNELS, ref_avg_int32);

	printf("Channel | Block average    | Moving average   | RMS\n");
	printf("        | float    int32   | float    int32   | float    int32\n");
	for (int i = 0; i < NUM_CHANNELS; i++) {
		/* Moving average of the whole channel, the last output covers the
		 * final window */
		vector<float> smoothed(DATA_LENGTH);
		vector<int32_t> smoothed_int32(DATA_LENGTH);
		ch_avg[i].moving_average(ch_data[i].x, smoothed, DATA_LENGTH);
		ch_avg_int32[i].moving_average(ch_data_int32[i], smoothed_int32,
									   DATA_LENGTH);

		/* The block functions over the whole channel. The int32 RMS is
		 * rounded to the nearest count */
		const float block = average(ch_data[i].x);
		const int32_t block_int32 = average(ch_data_int32[i]);
		const float block_rms = rms(ch_data[i].x);
		const int32_t rms_int32 = rms(ch_data_int32[i]);

		printf("%7d | %.5f  %.5f | %.5f  %.5f | %.5f  %.5f\n", i, block,
			   (double)block_int32 / INT32_SCALE, smoothed[DATA_LENGTH - 1],
			   (double)smoothed_int32[DATA_LENGTH - 1] / INT32_SCALE,
			   block_rms, (double)rms_int32 / INT32_SCALE);
	}
	return 0;
}

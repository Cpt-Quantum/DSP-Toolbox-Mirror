#include "../../cpp_wrapper.h"
#include <fstream>

using namespace std;

#define WAVELENGTH 1000
#define SAMPLE_RATE 250
#define NUM_CHANNELS 256 + 16

static const char output_filename[] = "results/output_file.csv";

int main() {
	vector<wave_properties_float_t> props;

	// configure basic waveform properties
	wave_properties_float_t test_prop = {.amplitude = 1,
										 .offset = 0,
										 .frequency = 1,
										 .phase = 0,
										 .function_type = FUNCTION_SINE};

	props.push_back(test_prop);

	// get the time series
	waveform_t<float> time_series(SAMPLE_RATE, WAVELENGTH);

	// loop, generating waveform x data with alterations to each channel
	vector<waveform_x<float>> ch_data;

	for (int i = 0; i < NUM_CHANNELS; i++) {
		// normalise
		float amplitude = (float)(i + 1) / float(NUM_CHANNELS);
		props[0].amplitude = amplitude;

		ch_data.push_back(waveform_x<float>(WAVELENGTH, props));
		ch_data[i].gen_waveform(time_series.t);
	}

	/* Print the results to file */
	ofstream out_file(output_filename);
	out_file << "Channels," << NUM_CHANNELS << ",Wavelength," << WAVELENGTH
			 << ",Sample rate," << SAMPLE_RATE << "\n";

	out_file << "Time";
	for (int i = 0; i < NUM_CHANNELS; i++) {
		out_file << "," << i;
	}
	out_file << "\n";

	for (int i = 0; i < WAVELENGTH; i++) {
		out_file << time_series.t[i];
		for (int j = 0; j < NUM_CHANNELS; j++) {
			out_file << "," << ch_data[j].x[i];
		}
		out_file << "\n";
	}
	out_file.close();

	return 0;
}

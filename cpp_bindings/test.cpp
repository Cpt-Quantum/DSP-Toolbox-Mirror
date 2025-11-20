#include <iostream>
#include "cpp_wrapper.h"
#include "../inc/waveform_gen.h"

using namespace std;


static const char output_filename[] = "output_file.csv";

int main()
{
	vector<wave_properties_t> props;
	
	//configure basic waveform properties
	wave_properties_t test_prop = {
		.amplitude = 1,
		.offset = 0,
		.frequency = 1,
		.phase = 0,
		.function_type = FUNCTION_SINE
	};

	props.push_back(test_prop);

	int wavelength = 1000;
	int sample_rate = 250;

	//get the time series
	waveform_t time_series(sample_rate, wavelength);
	
	//loop, generating waveform x data with alterations to each channel
	// vector<vector<float>> channel_data;
	float channel_data[4][wavelength];

	for(int i = 0; i < 4; i++)
	{
		props[0].amplitude = (float)i / 100 + 1;
		waveform_x waveform(wavelength, props);
		waveform.gen_waveform(time_series.t.data());

		// channel_data.pushback(waveform.x);

		//store in channel data
		for(int j = 0; j < wavelength; j++)
			channel_data[i][j] = waveform.x[j];
	}

	FILE* out_file = fopen(output_filename, "w");
	for (int i = 0; i < wavelength; i++)
	{
		fprintf(out_file, "%f,%f,%f,%f,%f\n", time_series.t[i], channel_data[0][i], channel_data[1][i], channel_data[2][i], channel_data[3][i]);
	}

	return 0;
}

#include <iostream>
#include "cpp_wrapper.h"
#include "../inc/waveform_gen.h"

using namespace std;

#define WAVELENGTH 1000
#define SAMPLE_RATE 250
#define NUM_CHANNELS 256 + 16

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

	//get the time series
	waveform_t time_series(SAMPLE_RATE, WAVELENGTH);
	
	//loop, generating waveform x data with alterations to each channel
	vector<waveform_x> ch_data;

	for(int i = 0; i < NUM_CHANNELS; i++)
	{
		//normalise
		float amplitude = (float)(i + 1) / float(NUM_CHANNELS);
		// cout << amplitude;
		// cout << "\n";
		props[0].amplitude = amplitude;

		ch_data.push_back(waveform_x(WAVELENGTH, props));
		ch_data[i].gen_waveform(time_series.t.data());
	}

	// cout << ch_data[0].x.size();
	// cout << "\n";

	FILE* out_file = fopen(output_filename, "w");
	
	fprintf(out_file, "Channels,%i,Wavelength,%i,Sample rate,%i\n", NUM_CHANNELS, WAVELENGTH, SAMPLE_RATE);
	
	fprintf(out_file, "Time");
	for(int i = 0; i < NUM_CHANNELS; i++)
	{
		fprintf(out_file, ",%i", i);
	}
	fprintf(out_file, "\n");
	
	for (int i = 0; i < WAVELENGTH; i++)
	{
		fprintf(out_file, "%f", time_series.t[i]);
		for(int j = 0; j < NUM_CHANNELS; j++)
		{
			fprintf(out_file, ",%f", ch_data[j].x[i]);
		}
		fprintf(out_file, "\n");
	}
	fclose(out_file);

	return 0;
}

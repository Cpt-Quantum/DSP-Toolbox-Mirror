#include <iostream>
#include "cpp_wrapper.h"
#include "../inc/waveform_gen.h"

using namespace std;

int main()
{
	vector<wave_properties_t> props;
	
	wave_properties_t test_prop = {
		.amplitude = 1,
		.offset = 0,
		.frequency = 1,
		.phase = 0,
		.function_type = FUNCTION_SINE
	};

	props.push_back(test_prop);

	int wavelength = 80000;
	int sample_rate = 259;

	waveform_t time_series(sample_rate, wavelength);

	// for(int i = 0; i < wavelength; i++)
	// {
	// 	cout << time_series.t[i];
	// }

	waveform_x test_waveform(wavelength, props);

	test_waveform.gen_waveform(time_series.t.data());

	cout << test_waveform.x.size();

	// for(int i = 0; i < wavelength; i++)
	// {
	// 	cout << test_waveform.x[i];
	// }

	return 0;
}

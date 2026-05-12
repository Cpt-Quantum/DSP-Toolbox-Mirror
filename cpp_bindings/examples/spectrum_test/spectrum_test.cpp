#include "../../cpp_wrapper.h"
#include <iostream>

using namespace std;

#define F_S 48000
#define DATA_LENGTH 2048
/* Spectrum settings */
#define N_FREQ_BINS 128
#define BASE_AMPLITUDE 1024
#define NOISE_VARIANCE 128

template <typename num_t> num_t spectrum_power_calc(vector<num_t> x) {
	/* Loop through all data in the vector and square the values */
	num_t accum = 0;
	for (unsigned int i = 0; i < x.size(); i++) {
		accum += x[i] * x[i];
	}
	/* Square root the accumulated value to get the power of the spectrum */
	return (num_t)sqrt(accum);
}

int main(void) {
	waveform_x<int64_t> x(DATA_LENGTH);
	waveform_t<int64_t> t(F_S, DATA_LENGTH);
	x.gen_spectrum(t.t, DATA_LENGTH, F_S, N_FREQ_BINS, AMPLITUDE_SPECTRUM_FLAT,
				   BASE_AMPLITUDE, NOISE_VARIANCE);
	cout << "Power of the spectrum: " << spectrum_power_calc(x.x) << endl;
}

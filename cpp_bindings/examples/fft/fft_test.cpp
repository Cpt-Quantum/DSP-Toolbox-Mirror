#include "../../cpp_data_io.h"
#include "../../cpp_wrapper_fft.h"
#include "../../cpp_wrapper_waveform.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/* Use Makefile flags, or fall back to defaults if compiled manually */
#ifndef F_S
#define F_S 16000
#endif
/* The transform size, a power of two */
#define DATA_LENGTH (32768) // Roughly 2s of data
/* Spectrum settings, the tones are every F_S / (2 * N_FREQ_BINS) Hz, with
 * their amplitudes following a bell curve centred on F_S / 4 */
#define N_FREQ_BINS 128
#define SPECTRUM_PROFILE AMPLITUDE_SPECTRUM_BANDPASS
#define BASE_AMPLITUDE 10
#define NOISE_VARIANCE 2

/* Define the data filenames */
#ifndef INPUT_BIN
#define INPUT_BIN "results/input_data.bin"
#endif
#ifndef OUTPUT_BIN
#define OUTPUT_BIN "results/fft_data.bin"
#endif
const string input_filename = INPUT_BIN;
const string output_filename = OUTPUT_BIN;

int main() {
	/* Generate the input signal */
	cout << "Generating " << DATA_LENGTH << " samples of test data..." << endl;
	waveform_x<float> x(DATA_LENGTH);
	waveform_t<float> t(F_S, DATA_LENGTH);
	x.gen_spectrum(t.t, DATA_LENGTH, F_S, N_FREQ_BINS, SPECTRUM_PROFILE,
				   BASE_AMPLITUDE, NOISE_VARIANCE, 0);

	/* Create the plan, which calculates the twiddle factors once so they can
	 * be reused by every transform of this size. The input is real, so the
	 * real FFT is used, fft<float> is the equivalent for complex input */
	rfft<float> plan(DATA_LENGTH);

	/* Transform the signal, giving bins 0 to DATA_LENGTH / 2. Bin k is at
	 * k * F_S / DATA_LENGTH Hz */
	cout << "Performing the FFT..." << endl;
	vector<rfft<float>::complex_type> bins;
	plan.forward(x.x, bins);

	/* Write the input samples and the bins to file */
	dsp_bin_write<float> input_write(input_filename, DATA_LENGTH);
	dsp_bin_write<rfft<float>::complex_type> output_write(output_filename,
														   bins.size());
	if (!input_write.open_stream() || !output_write.open_stream()) {
		cout << "Failed to open input/output data files." << endl;
		return -1;
	}
	input_write.write_chunk(x.x);
	output_write.write_chunk(bins);
	input_write.close_stream();
	output_write.close_stream();
	cout << "Wrote " << bins.size() << " bins to " << output_filename << endl;

	return 0;
}

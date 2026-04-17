#ifndef CPP_WRAPPER_H
#define CPP_WRAPPER_H

#include "cpp_data_io.h"
#include "cpp_wrapper.h"
#include <chrono>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>

using namespace std;

/* Define data constants */
/* Use Makefile flags, or fall back to defaults if compiled manually */
#ifndef F_S
#define F_S 16000
#endif
#ifndef N_CHANNELS
#define N_CHANNELS 288
#endif
#ifndef N_TAPS
#define N_TAPS 32
#endif
#ifndef DECIMATION_RATE
#define DECIMATION_RATE 4
#endif
/* These constants are only used in the C++ program so aren't specified in the
 * makefile */
#define CHUNK_SIZE N_TAPS
#define N_CHUNKS 2000
#define DATA_LENGTH (CHUNK_SIZE * N_CHUNKS)
#define FREQ_BINS 200
#define NOISE_VARIANCE 1

template <typename num_t> class data_filter {
  private:
	unsigned int chunk_size;
	unsigned int n_channels;
	unsigned int n_chunks;
	unsigned int sample_rate;

	waveform_t<num_t> t;
	data_io<num_t> data;

  public:
	/* Constructor */
	data_filter(string _ifilename, string _ofilename, unsigned int _chunk_size,
				unsigned int _freq_bins, unsigned int _n_channels,
				unsigned _noise_variance, unsigned int _sample_rate,
				unsigned int _wave_length)
		: chunk_size(_chunk_size), n_channels(_n_channels),
		  sample_rate(_sample_rate), t(_sample_rate, _wave_length),
		  data(_ifilename, _ofilename, _chunk_size, _freq_bins, _n_channels,
			   _noise_variance, _sample_rate, _wave_length) {
		n_chunks = _wave_length / chunk_size;
	};
	/* Wrapper function to expose any needed functions from data_io */
	unsigned int generate_input_file() { return data.generate_input_file(); }
	bool input_data_exists() { return data.input_data_exists(); }
	/* Function to filter the input data and work out performance of the code */
	void run_benchmark(const string &json_filename,
					   const string &output_filename, uint8_t decimation_rate) {

		/* Create the filter objects */
		cout << "Loading FIR filter from " << json_filename << "..." << endl;
		json_filter_parse json_object(json_filename);
		fir<float> ref_filt(json_object.get_node("fir"));

		vector<fir<float>> channel_filters;
		channel_filters.reserve(n_channels);
		for (unsigned int i = 0; i < n_channels; ++i) {
			channel_filters.push_back(ref_filt);
		}

		/* Open the input and output data files */
		if (!data.open_streams()) {
			cout << "Failed to open input/output data files." << endl;
			return;
		}

		/* Set up the data chunk arrays */
		vector<float> input_chunk(chunk_size);
		unsigned int out_chunk_size = chunk_size / decimation_rate;
		vector<float> output_chunk(out_chunk_size);

		long long total_compute_microseconds = 0;
		unsigned int chunks_processed = 0;

		cout << "Running sequential read, compute, and write benchmark..."
			 << endl;

		while (true) {
			bool eof_reached = false;

			for (unsigned int ch = 0; ch < n_channels; ++ch) {
				/* Read input from binary file */
				if (!data.read_chunk(input_chunk, chunk_size)) {
					eof_reached = true;
					break;
				}

				/* Start timer for benchmarking only around computation section
				 */
				auto start = chrono::high_resolution_clock::now();

				channel_filters[ch].fir_decimate(input_chunk, output_chunk,
												 chunk_size, decimation_rate);

				/* Stop the benchmarking timer */
				auto end = chrono::high_resolution_clock::now();
				total_compute_microseconds +=
					chrono::duration_cast<chrono::microseconds>(end - start)
						.count();

				/* Write the output to file */
				data.write_chunk(output_chunk, out_chunk_size);
			}

			if (eof_reached)
				break;
			chunks_processed++;
		}

		/* Close the input and output files */
		data.close_streams();

		/* Print out benchmark results */
		double total_time_sec = total_compute_microseconds / 1000000.0;
		double simulated_data_sec =
			(double)(chunks_processed * chunk_size) / sample_rate;

		cout << "\n--- Benchmark Results ---" << endl;
		cout << "Time Steps Processed: " << chunks_processed << " ("
			 << (chunks_processed * n_channels) << " total channel chunks)"
			 << endl;
		cout << "Simulated Data Time: " << simulated_data_sec << " seconds"
			 << endl;
		cout << "Total Compute Time:   " << total_time_sec << " seconds"
			 << endl;

		if (total_time_sec < simulated_data_sec) {
			cout << "\nRESULT: PASS." << endl;
			cout << "CPU Load: "
				 << (total_time_sec / simulated_data_sec) * 100.0 << "%"
				 << endl;
		} else {
			cout << "\nRESULT: FAIL. The filter cannot keep up with real-time."
				 << endl;
		}
	}
};

/* Define the data filenames */
const string input_filename = "results/input_data.bin";
const string output_filename = "results/decimated_data.bin";
/* Filter spec filename */
const string json_filename("fir.json");

int main(void) {
	/* Create the class that handles the file IO */
	data_filter<float> eeg(input_filename, output_filename, CHUNK_SIZE,
						   FREQ_BINS, N_CHANNELS, NOISE_VARIANCE, F_S,
						   DATA_LENGTH);

	/* Only generate data if there isn't a file already generated */
	bool input_data_exists = eeg.input_data_exists();
	if (input_data_exists) {
		cout << "Input data file found in: " << input_filename
			 << ", skipping data generation step." << endl;
	} else {
		if (eeg.generate_input_file() != 0) {
			return -1;
		}
	}

	/* Run the benchmark */
	eeg.run_benchmark(json_filename, output_filename, DECIMATION_RATE);

	return 0;
}

#endif /* CPP_WRAPPER_H */

#ifndef CPP_WRAPPER_H
#define CPP_WRAPPER_H

#include "../../cpp_data_io.h"
#include "../../cpp_wrapper.h"
#include <chrono>
#include <iosfwd>
#include <iostream>
#include <string>
#include <vector>

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
#define N_TAPS 100
#endif
#ifndef DECIMATION_RATE
#define DECIMATION_RATE 4
#endif
#ifndef REGEN_INPUT_FILE
#define REGEN_INPUT_FILE false
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

	dsp_bin_gen<num_t> data_gen;
	dsp_bin_write<num_t> data_write;
	dsp_bin_read<num_t> data_read;

  public:
	/* Constructor */
	data_filter(string _ifilename, string _ofilename, unsigned int _chunk_size,
				unsigned int _n_chunks, unsigned int _freq_bins,
				unsigned int _n_channels, num_t _base_amplitude,
				num_t _noise_variance, unsigned int _sample_rate)
		: chunk_size(_chunk_size), n_channels(_n_channels), n_chunks(_n_chunks),
		  sample_rate(_sample_rate),
		  data_gen(_ifilename, _chunk_size, _n_chunks, _freq_bins, _n_channels,
				   _base_amplitude, _noise_variance, _sample_rate),
		  data_write(_ofilename, _chunk_size),
		  data_read(_ifilename, _chunk_size) {};
	/* Wrapper function to expose any needed functions from data_io */
	unsigned int generate_input_file(bool regenerate_if_exists) {
		return data_gen.generate_input_file(regenerate_if_exists);
	}
	/* Function to filter the input data and work out performance of the code
	 * for each of the memory organisation paths */
	void run_benchmark(const string &json_filename,
					   const string &indep_output_filename,
					   uint8_t decimation_rate) {

		/* Create the reference filter object, copied per channel in each path
		 */
		cout << "Loading FIR filter from " << json_filename << "..." << endl;
		json_filter_parse json_object(json_filename);
		fir<float> ref_filt(json_object.get_node("fir"));

		unsigned int seq_steps = 0;
		long long seq_ns = run_sequential(ref_filt, decimation_rate, seq_steps);
		if (seq_ns < 0)
			return;
		print_results("A: Sequential (single reused chunk buffer)", seq_ns,
					  seq_steps);

		unsigned int ind_steps = 0;
		long long ind_ns = run_independent(ref_filt, indep_output_filename,
										   decimation_rate, ind_steps);
		if (ind_ns < 0)
			return;
		print_results("B: Independent per-channel buffers", ind_ns, ind_steps);

		cout << "\n--- Comparison ---" << endl;
		cout << "B / A compute time ratio: " << (double)ind_ns / seq_ns << endl;
	}

  private:
	/* Path A: read, decimate and write one chunk at a time, reusing a single
	 * chunk buffer for every channel. Returns the compute time in ns, or -1 on
	 * failure */
	long long run_sequential(const fir<float> &ref_filt,
							 uint8_t decimation_rate,
							 unsigned int &chunks_processed) {
		vector<fir<float>> channel_filters(n_channels, ref_filt);

		/* Open the input and output data files */
		if (!data_read.open_stream() || !data_write.open_stream()) {
			cout << "Failed to open input/output data files." << endl;
			return -1;
		}

		/* Set up the data chunk arrays */
		vector<float> input_chunk(chunk_size);
		unsigned int out_chunk_size = chunk_size / decimation_rate;
		vector<float> output_chunk(out_chunk_size);
		/* Adjust the set chunk size for the output data */
		data_write.chunk_size = out_chunk_size;

		long long total_compute_ns = 0;
		chunks_processed = 0;

		cout << "\nRunning path A: sequential read, compute, and write..."
			 << endl;

		while (true) {
			bool eof_reached = false;

			for (unsigned int ch = 0; ch < n_channels; ++ch) {
				/* Read input from binary file */
				if (!data_read.read_chunk(input_chunk)) {
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
				total_compute_ns +=
					chrono::duration_cast<chrono::nanoseconds>(end - start)
						.count();

				/* Write the output to file */
				data_write.write_chunk(output_chunk);
			}

			if (eof_reached)
				break;
			chunks_processed++;
		}

		/* Close the input and output files */
		data_write.close_stream();
		data_read.close_stream();

		return total_compute_ns;
	}

	/* Path B: each channel's whole signal is held in its own contiguous
	 * buffer. The data is processed in the same order as path A (time step
	 * outer, channel inner), so only the memory organisation differs. Returns
	 * the compute time in ns, or -1 on failure */
	long long run_independent(const fir<float> &ref_filt,
							  const string &output_filename,
							  uint8_t decimation_rate,
							  unsigned int &chunks_processed) {
		vector<fir<float>> channel_filters(n_channels, ref_filt);
		unsigned int out_chunk_size = chunk_size / decimation_rate;

		/* Allocate the per-channel input and output buffers */
		vector<vector<float>> in_ch(n_channels,
									vector<float>(n_chunks * chunk_size));
		vector<vector<float>> out_ch(n_channels,
									 vector<float>(n_chunks * out_chunk_size));

		/* De-interleave the time-major input file into the per-channel
		 * buffers (untimed) */
		if (!data_read.open_stream()) {
			cout << "Failed to open input data file." << endl;
			return -1;
		}
		vector<float> input_chunk(chunk_size);
		chunks_processed = 0;
		for (unsigned int step = 0; step < n_chunks; ++step) {
			bool eof_reached = false;
			for (unsigned int ch = 0; ch < n_channels; ++ch) {
				if (!data_read.read_chunk(input_chunk)) {
					eof_reached = true;
					break;
				}
				copy(input_chunk.begin(), input_chunk.end(),
					 in_ch[ch].begin() + (size_t)step * chunk_size);
			}
			if (eof_reached)
				break;
			chunks_processed++;
		}
		data_read.close_stream();

		cout << "\nRunning path B: compute from independent per-channel "
				"buffers..."
			 << endl;

		long long total_compute_ns = 0;
		for (unsigned int step = 0; step < chunks_processed; ++step) {
			for (unsigned int ch = 0; ch < n_channels; ++ch) {
				/* Start timer for benchmarking only around computation section
				 */
				auto start = chrono::high_resolution_clock::now();

				channel_filters[ch].fir_decimate(
					&in_ch[ch][(size_t)step * chunk_size],
					&out_ch[ch][(size_t)step * out_chunk_size], chunk_size,
					decimation_rate);

				/* Stop the benchmarking timer */
				auto end = chrono::high_resolution_clock::now();
				total_compute_ns +=
					chrono::duration_cast<chrono::nanoseconds>(end - start)
						.count();
			}
		}

		/* Re-interleave the output into the time-major file layout so it can
		 * be compared against path A (untimed) */
		dsp_bin_write<num_t> indep_write(output_filename, out_chunk_size);
		if (!indep_write.open_stream()) {
			cout << "Failed to open output data file." << endl;
			return -1;
		}
		vector<float> output_chunk(out_chunk_size);
		for (unsigned int step = 0; step < chunks_processed; ++step) {
			for (unsigned int ch = 0; ch < n_channels; ++ch) {
				auto it = out_ch[ch].begin() + (size_t)step * out_chunk_size;
				copy(it, it + out_chunk_size, output_chunk.begin());
				indep_write.write_chunk(output_chunk);
			}
		}
		indep_write.close_stream();

		return total_compute_ns;
	}

	/* Print out benchmark results for a single path */
	void print_results(const string &name, long long compute_ns,
					   unsigned int chunks_processed) {
		double total_time_sec = compute_ns / 1e9;
		double simulated_data_sec =
			(double)chunks_processed * chunk_size / sample_rate;

		cout << "\n--- Benchmark Results: " << name << " ---" << endl;
		cout << "Time Steps Processed: " << chunks_processed << " ("
			 << (chunks_processed * n_channels) << " total channel chunks)"
			 << endl;
		cout << "Simulated Data Time: " << simulated_data_sec << " seconds"
			 << endl;
		cout << "Total Compute Time:   " << total_time_sec << " seconds"
			 << endl;

		if (total_time_sec < simulated_data_sec) {
			cout << "RESULT: PASS." << endl;
			cout << "CPU Load: "
				 << (total_time_sec / simulated_data_sec) * 100.0 << "%"
				 << endl;
		} else {
			cout << "RESULT: FAIL. The filter cannot keep up with real-time."
				 << endl;
		}
	}
};

/* Define the data filenames */
const string input_filename = "results/input_data.bin";
const string output_filename = "results/decimated_data.bin";
const string indep_output_filename = "results/decimated_data_indep.bin";
/* Filter spec filename */
const string json_filename("fir.json");

int main(void) {
	/* Create the class that handles the file IO */
	data_filter<float> eeg(input_filename, output_filename, CHUNK_SIZE,
						   N_CHUNKS, FREQ_BINS, N_CHANNELS, 1, NOISE_VARIANCE,
						   F_S);

	/* Generate the input data and run the benchmark */
	if (eeg.generate_input_file(REGEN_INPUT_FILE) != 0) {
		return -1;
	}
	eeg.run_benchmark(json_filename, indep_output_filename, DECIMATION_RATE);

	return 0;
}

#endif /* CPP_WRAPPER_H */

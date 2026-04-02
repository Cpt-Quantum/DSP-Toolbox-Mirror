#include "cpp_wrapper.h"
#include <chrono>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>

using namespace std;

/* Define data constants */
#define F_S (16000)
#define N_CHANNELS (256 + 32)
#define DECIMATION_RATE 4
#define N_TAPS (32)
#define CHUNK_SIZE N_TAPS
#define N_CHUNKS 2000
#define DATA_LENGTH (CHUNK_SIZE * N_CHUNKS)
#define FREQ_BINS 200

class data_filter {
  private:
	string ifilename;
	unsigned int sample_rate;
	unsigned int wave_length;
	unsigned int freq_bins;
	unsigned int n_channels;
	unsigned int chunk_size;
	unsigned int n_chunks;

	waveform_t<float> t;

  public:
	/* Constructor */
	data_filter(string _ifilename, unsigned int _sample_rate,
				unsigned int _wave_length, unsigned int _freq_bins,
				unsigned int _n_channels, unsigned int _chunk_size)
		: ifilename(_ifilename), sample_rate(_sample_rate),
		  wave_length(_wave_length), freq_bins(_freq_bins),
		  n_channels(_n_channels), chunk_size(_chunk_size),
		  t(_sample_rate, _wave_length) {
		n_chunks = wave_length / chunk_size;
	};
	/* Function to create the input waveforms and dump to a binary file for
	 * loading later in the program */
	uint8_t generate_input_bin_file() {
		cout << "Generating input data binary file" << endl;
		/* Open the output file */
		fstream in_file_wr{ifilename, in_file_wr.out | in_file_wr.binary};
		if (!in_file_wr.is_open()) {
			cout
				<< "Failed to open input data file for writing, exiting program"
				<< endl;
			return -1;
		}
		/* Preallocate the file size */
		in_file_wr.seekp(
			(size_t)(n_chunks * n_channels * chunk_size * sizeof(float) - 1));
		in_file_wr.write("", 1);
		/* Setup the channels and print them to file */
		for (int i = 0; i < N_CHANNELS; i++) {
			waveform_x<float> x(wave_length);
			x.gen_spectrum(t.t, wave_length, sample_rate, freq_bins,
						   AMPLITUDE_SPECTRUM_FLAT);
			/* The binary file output is grouped into chunks for each channel,
			 * rather than storing each channel contiguously. This drastically
			 * improves performance when filtering there data, as that is done
			 * per chunk. */
			for (unsigned int j = 0; j < n_chunks; j++) {
				size_t element_start_index =
					(j * n_channels * chunk_size) + (i * chunk_size);
				streampos byte_offset = element_start_index * sizeof(float);
				/* Seek to the offset */
				in_file_wr.seekp(byte_offset, ios::beg);
				/* Write the chunk to file */
				in_file_wr.write((const char *)&x.x[j * chunk_size],
								 chunk_size * sizeof(float));
			}
		}
		/* Close the file */
		in_file_wr.close();
		return 0;
	}
	/* Function to filter the input data and work out performance of the code */
	void run_benchmark(const string &json_filename,
					   const string &output_filename, uint8_t decimation_rate) {
		cout << "Loading FIR filter from " << json_filename << "..." << endl;

		json_filter_parse json_object(json_filename);
		fir<float> ref_filt(json_object.get_node("fir"));

		vector<fir<float>> channel_filters;
		channel_filters.reserve(n_channels);
		for (unsigned int i = 0; i < n_channels; ++i) {
			channel_filters.push_back(ref_filt);
		}

		/* Open input for reading, and output for writing */
		ifstream in_file_rd(ifilename, ios::in | ios::binary);
		ofstream out_file_wr(output_filename, ios::out | ios::binary);

		if (!in_file_rd.is_open() || !out_file_wr.is_open()) {
			cout << "Failed to open input or output data files." << endl;
			return;
		}

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
				// 1. Read Input
				if (!in_file_rd.read((char *)input_chunk.data(),
									 chunk_size * sizeof(float))) {
					eof_reached = true;
					break;
				}

				// 2. Start Timer (Strictly compute)
				auto start = chrono::high_resolution_clock::now();

				channel_filters[ch].fir_decimate(input_chunk, output_chunk,
												 chunk_size, decimation_rate);

				// 3. Stop Timer
				auto end = chrono::high_resolution_clock::now();
				total_compute_microseconds +=
					chrono::duration_cast<chrono::microseconds>(end - start)
						.count();

				// 4. Write Output (Outside the timer!)
				out_file_wr.write((const char *)output_chunk.data(),
								  out_chunk_size * sizeof(float));
			}

			if (eof_reached)
				break;
			chunks_processed++;
		}

		in_file_rd.close();
		out_file_wr.close();

		/* --- Display Results --- */
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
	data_filter eeg(input_filename, F_S, DATA_LENGTH, FREQ_BINS, N_CHANNELS,
					CHUNK_SIZE);
	if (eeg.generate_input_bin_file() != 0) {
		return -1;
	}

	/* Run the benchmark (assuming a decimation rate of 2 for this example) */
	eeg.run_benchmark(json_filename, output_filename, DECIMATION_RATE);

	return 0;
}

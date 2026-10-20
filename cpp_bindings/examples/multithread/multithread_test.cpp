#include "../../cpp_data_io.h"
#include "../../cpp_wrapper.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
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
#ifndef N_THREADS
#define N_THREADS 4
#endif
#ifndef CHUNK_SIZE
#define CHUNK_SIZE 100
#endif
#ifndef REGEN_INPUT_FILE
#define REGEN_INPUT_FILE false
#endif
/* These constants are only used in the C++ program so aren't specified in the
 * makefile */
#define N_CHUNKS 2000
#define FREQ_BINS 200
#define NOISE_VARIANCE 1

/* Each channel's whole signal held in its own contiguous buffer */
template <typename num_t> using channel_data = vector<vector<num_t>>;

template <typename num_t> class mt_filter {
  private:
	unsigned int chunk_size;
	unsigned int n_channels;
	unsigned int n_chunks;
	unsigned int sample_rate;
	unsigned int chunks_loaded = 0;

	dsp_bin_gen<num_t> data_gen;
	dsp_bin_read<num_t> data_read;

	/* Per-channel input data, loaded once and shared by every run */
	channel_data<num_t> in_ch;

  public:
	/* Constructor */
	mt_filter(string _ifilename, unsigned int _chunk_size,
			  unsigned int _n_chunks, unsigned int _freq_bins,
			  unsigned int _n_channels, num_t _base_amplitude,
			  num_t _noise_variance, unsigned int _sample_rate)
		: chunk_size(_chunk_size), n_channels(_n_channels), n_chunks(_n_chunks),
		  sample_rate(_sample_rate),
		  data_gen(_ifilename, _chunk_size, _n_chunks, _freq_bins, _n_channels,
				   _base_amplitude, _noise_variance, _sample_rate),
		  data_read(_ifilename, _chunk_size) {};
	/* Wrapper function to expose any needed functions from data_io */
	unsigned int generate_input_file(bool regenerate_if_exists) {
		return data_gen.generate_input_file(regenerate_if_exists);
	}

	/* De-interleave the time-major input file into the per-channel buffers.
	 * Returns false on failure */
	bool load_input() {
		in_ch.assign(n_channels, vector<num_t>((size_t)n_chunks * chunk_size));
		if (!data_read.open_stream()) {
			cout << "Failed to open input data file." << endl;
			return false;
		}
		vector<num_t> input_chunk(chunk_size);
		chunks_loaded = 0;
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
			chunks_loaded++;
		}
		data_read.close_stream();
		return chunks_loaded > 0;
	}

	/* Run one filter type single threaded and then split across n_threads,
	 * check the outputs match and print the results. filter_chunk is called
	 * as filter_chunk(filter, in, out, length) for each channel chunk */
	template <typename filt_t, typename filter_fn>
	bool run_benchmark(const string &name, const filt_t &ref_filt,
					   unsigned int n_threads, const string &output_filename,
					   filter_fn filter_chunk) {
		channel_data<num_t> out_single, out_multi;

		cout << "\nRunning " << name << " on 1 thread..." << endl;
		long long single_ns = run(ref_filt, 1, out_single, filter_chunk);
		cout << "Running " << name << " on " << n_threads << " threads..."
			 << endl;
		long long multi_ns = run(ref_filt, n_threads, out_multi, filter_chunk);

		/* Splitting the channels across threads mustn't change the result */
		bool match = (out_single == out_multi);

		print_results(name, n_threads, single_ns, multi_ns, match);
		write_output(output_filename, out_single);
		return match;
	}

  private:
	/* Filter every channel, with the channels split into n_threads contiguous
	 * groups that each run on their own thread. Each channel gets its own copy
	 * of the reference filter. Returns the wall clock compute time in ns */
	template <typename filt_t, typename filter_fn>
	long long run(const filt_t &ref_filt, unsigned int n_threads,
				  channel_data<num_t> &out_ch, filter_fn filter_chunk) {
		vector<filt_t> channel_filters(n_channels, ref_filt);
		out_ch.assign(n_channels,
					  vector<num_t>((size_t)chunks_loaded * chunk_size));

		/* Process the channels in [ch_start, ch_end) in the same order as the
		 * decimation example (time step outer, channel inner) */
		auto worker = [&](unsigned int ch_start, unsigned int ch_end) {
			for (unsigned int step = 0; step < chunks_loaded; ++step) {
				size_t offset = (size_t)step * chunk_size;
				for (unsigned int ch = ch_start; ch < ch_end; ++ch) {
					filter_chunk(channel_filters[ch], &in_ch[ch][offset],
								 &out_ch[ch][offset], chunk_size);
				}
			}
		};

		auto start = chrono::steady_clock::now();
		if (n_threads <= 1) {
			worker(0, n_channels);
		} else {
			/* Spread any remainder so group sizes differ by at most one */
			vector<thread> threads;
			unsigned int ch_start = 0;
			for (unsigned int t = 0; t < n_threads; ++t) {
				unsigned int count =
					n_channels / n_threads + (t < n_channels % n_threads);
				threads.emplace_back(worker, ch_start, ch_start + count);
				ch_start += count;
			}
			for (auto &th : threads)
				th.join();
		}
		auto end = chrono::steady_clock::now();

		return chrono::duration_cast<chrono::nanoseconds>(end - start).count();
	}

	/* Re-interleave the output into the time-major file layout used by the
	 * input file, so it can be plotted (untimed) */
	void write_output(const string &output_filename,
					  const channel_data<num_t> &out_ch) {
		dsp_bin_write<num_t> data_write(output_filename, chunk_size);
		if (!data_write.open_stream()) {
			cout << "Failed to open output data file." << endl;
			return;
		}
		vector<num_t> output_chunk(chunk_size);
		for (unsigned int step = 0; step < chunks_loaded; ++step) {
			for (unsigned int ch = 0; ch < n_channels; ++ch) {
				auto it = out_ch[ch].begin() + (size_t)step * chunk_size;
				copy(it, it + chunk_size, output_chunk.begin());
				data_write.write_chunk(output_chunk);
			}
		}
		data_write.close_stream();
	}

	/* Print out the single and multi-threaded results for one filter */
	void print_results(const string &name, unsigned int n_threads,
					   long long single_ns, long long multi_ns, bool match) {
		double simulated_data_sec =
			(double)chunks_loaded * chunk_size / sample_rate;
		double single_sec = single_ns / 1e9;
		double multi_sec = multi_ns / 1e9;
		double speedup = (double)single_ns / multi_ns;

		cout << "\n--- Benchmark Results: " << name << " ---" << endl;
		cout << "Time Steps Processed: " << chunks_loaded << " ("
			 << (chunks_loaded * n_channels) << " total channel chunks)"
			 << endl;
		cout << "Simulated Data Time:  " << simulated_data_sec << " seconds"
			 << endl;
		cout << "1 thread:   " << single_sec << " seconds, real-time load "
			 << (single_sec / simulated_data_sec) * 100.0 << "%" << endl;
		cout << n_threads << " threads:  " << multi_sec
			 << " seconds, real-time load "
			 << (multi_sec / simulated_data_sec) * 100.0 << "%" << endl;
		cout << "Speedup:    " << speedup << "x (" << speedup / n_threads * 100.0
			 << "% parallel efficiency)" << endl;
		cout << "Outputs:    "
			 << (match ? "MATCH, multi-threaded output is bit-identical"
					   : "MISMATCH, multi-threaded output differs")
			 << endl;
	}
};

/* Define the data filenames */
const string input_filename = "results/input_data.bin";
const string fir_output_filename = "results/fir_output.bin";
const string iir_sos_output_filename = "results/iir_sos_output.bin";
const string iir_ba_output_filename = "results/iir_ba_output.bin";
/* Filter spec filename */
const string json_filename("filters.json");

int main(void) {
	/* Create the class that handles the file IO */
	mt_filter<float> eeg(input_filename, CHUNK_SIZE, N_CHUNKS, FREQ_BINS,
						 N_CHANNELS, 1, NOISE_VARIANCE, F_S);

	/* Generate and load the input data */
	if (eeg.generate_input_file(REGEN_INPUT_FILE) != 0 || !eeg.load_input()) {
		return -1;
	}

	/* Load the reference filters, copied per channel in each run */
	cout << "Loading FIR and IIR filters from " << json_filename << "..."
		 << endl;
	json_filter_parse json_object(json_filename);
	fir<float> ref_fir(json_object.get_node("fir"));
	iir_sos<float> ref_sos(json_object.get_node("iir_sos"));
	iir_ba<float> ref_ba(json_object.get_node("iir_ba"));
	cout << "FIR: " << ref_fir.get_num_taps() << " taps, IIR (SOS): "
		 << ref_sos.get_num_sections()
		 << " second order sections, IIR (direct form): order "
		 << ref_ba.get_num_taps() - 1 << endl;

	bool fir_ok = eeg.run_benchmark(
		"FIR", ref_fir, N_THREADS, fir_output_filename,
		[](fir<float> &f, float *in, float *out, uint32_t len) {
			f.fir_filter(in, out, len);
		});
	bool sos_ok = eeg.run_benchmark(
		"IIR (SOS)", ref_sos, N_THREADS, iir_sos_output_filename,
		[](iir_sos<float> &f, float *in, float *out, uint32_t len) {
			f.iir_filter(in, out, len);
		});
	bool ba_ok = eeg.run_benchmark(
		"IIR (direct form)", ref_ba, N_THREADS, iir_ba_output_filename,
		[](iir_ba<float> &f, float *in, float *out, uint32_t len) {
			f.iir_filter(in, out, len);
		});

	return (fir_ok && sos_ok && ba_ok) ? 0 : 1;
}

#ifndef CPP_DATA_IO_H
#define CPP_DATA_IO_H

#include "cpp_wrapper.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

template <typename num_t> class data_io {
  private:
	std::string ifilename;
	std::string ofilename;
	unsigned int chunk_size;
	unsigned int freq_bins;
	unsigned int n_channels;
	unsigned int n_chunks;
	num_t noise_variance;
	unsigned int sample_rate;
	unsigned int wave_length;

	waveform_t<num_t> t;

	std::ifstream in_stream;
	std::ofstream out_stream;

  public:
	/* Function for checking if data input file already exists */
	bool input_data_exists() {
		std::ifstream ifile(ifilename, std::ios::in | std::ios::binary);
		if (ifile.is_open()) {
			return true;
		} else {
			return false;
		}
	}
	/* Binary output file generation */
	unsigned int generate_input_file() {
		std::cout << "Generating input data binary file" << std::endl;
		/* Open the output file */
		std::fstream in_file_wr{ifilename, std::ios::out | std::ios::binary};
		if (!in_file_wr.is_open()) {
			std::cout
				<< "Failed to open input data file for writing, exiting program"
				<< std::endl;
			return -1;
		}
		/* Preallocate the file size */
		in_file_wr.seekp(
			(size_t)(n_chunks * n_channels * chunk_size * sizeof(num_t) - 1));
		in_file_wr.write("", 1);
		/* Setup the channels and print them to file */
		for (unsigned int i = 0; i < n_channels; i++) {
			waveform_x<num_t> x(wave_length);
			x.gen_spectrum(t.t, wave_length, sample_rate, freq_bins,
						   AMPLITUDE_SPECTRUM_FLAT, 0);
			/* The binary file output is grouped into chunks for each channel,
			 * rather than storing each channel contiguously. This drastically
			 * improves performance when filtering there data, as that is done
			 * per chunk. */
			for (unsigned int j = 0; j < n_chunks; j++) {
				size_t element_start_index =
					(j * n_channels * chunk_size) + (i * chunk_size);
				std::streampos byte_offset =
					element_start_index * sizeof(num_t);
				/* Seek to the offset */
				in_file_wr.seekp(byte_offset, std::ios::beg);
				/* Write the chunk to file */
				in_file_wr.write((const char *)&x.x[j * chunk_size],
								 chunk_size * sizeof(num_t));
			}
		}
		/* Close the file */
		in_file_wr.close();
		return 0;
	}

	/* Functions for opening and closing the files for chunk reads/writes */
	bool open_streams() {
		in_stream.open(ifilename, std::ios::in | std::ios::binary);
		out_stream.open(ofilename, std::ios::out | std::ios::binary);
		return in_stream.is_open() && out_stream.is_open();
	}
	void close_streams() {
		if (in_stream.is_open())
			in_stream.close();
		if (out_stream.is_open())
			out_stream.close();
	}

	/* Functions for reading/writing chunks from the opened files */
	bool read_chunk(std::vector<float> &chunk_data, unsigned int size) {
		return (bool)in_stream.read(reinterpret_cast<char *>(chunk_data.data()),
									size * sizeof(float));
	}
	bool write_chunk(const std::vector<float> &chunk_data, unsigned int size) {
		return (bool)out_stream.write(
			reinterpret_cast<const char *>(chunk_data.data()),
			size * sizeof(float));
	}

	/* Constructor(s) */
	data_io(std::string _ifilename, std::string _ofilename,
			unsigned int _chunk_size, unsigned int _freq_bins,
			unsigned int _n_channels, num_t _noise_variance,
			unsigned int _sample_rate, unsigned int _wave_length)
		: ifilename(_ifilename), ofilename(_ofilename), chunk_size(_chunk_size),
		  freq_bins(_freq_bins), n_channels(_n_channels),
		  noise_variance(_noise_variance), sample_rate(_sample_rate),
		  wave_length(_wave_length), t(_sample_rate, _wave_length) {
		n_chunks = wave_length / chunk_size;
	};
	/* Destructor */
	~data_io() { close_streams(); };
};

class data_file_io {};

#endif /* CPP_DATA_IO_H */

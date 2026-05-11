#ifndef CPP_DATA_IO_H
#define CPP_DATA_IO_H

#include "cpp_wrapper.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

class dsp_bin_base {
  private:
	std::string filename;

  protected:
	std::fstream stream_handle;

  public:
	/* Stream handling functions */
	bool open_stream_read() {
		stream_handle.open(filename, std::ios::in | std::ios::binary);
		return stream_handle.is_open();
	}
	bool open_stream_write() {
		stream_handle.open(filename, std::ios::out | std::ios::binary);
		return stream_handle.is_open();
	}
	void close_stream() {
		if (stream_handle.is_open())
			stream_handle.close();
	}
	bool file_exists() {
		stream_handle.open(filename, std::ios::in | std::ios::binary);
		bool ret = stream_handle.is_open();
		stream_handle.close();
		return ret;
	}
	/* Constructor/ Destructor */
	dsp_bin_base(std::string _filename) : filename(_filename) {}
	~dsp_bin_base() { close_stream(); }
};

template <typename num_t> class dsp_bin_gen : dsp_bin_base {
  private:
	/* File access settings */
	unsigned int chunk_size;
	unsigned int n_chunks;
	/* Frequency spectrum settings */
	unsigned int freq_bins;
	unsigned int n_channels;
	num_t base_amplitude;
	num_t noise_variance;
	unsigned int sample_rate;

  public:
	unsigned int generate_input_file(bool regenerate_if_exists) {
		if (!regenerate_if_exists && this->file_exists()) {
			/* File exists already and the flag for regenerating the file aren't
			 * set so just exit */
			std::cout << "Reference data file found, and flag to regenerate is "
						 "turned off, skipping reference data generation."
					  << std::endl;
			return 0;
		} else {
			std::cout << "Generating input data binary file" << std::endl;
			/* Open the output file */
			if (!this->open_stream_write()) {
				std::cout << "Failed to open input data file for writing, "
							 "exiting program"
						  << std::endl;
				return -1;
			}
			/* Preallocate the file size */
			this->stream_handle.seekp(
				(size_t)(n_chunks * n_channels * chunk_size * sizeof(num_t) -
						 1));
			this->stream_handle.write("", 1);
			/* Setup the channels and print them to file */
			unsigned int wave_length = n_chunks * chunk_size;
			waveform_t<num_t> t(sample_rate, wave_length);
			for (unsigned int i = 0; i < n_channels; i++) {
				waveform_x<num_t> x(wave_length);
				x.gen_spectrum(t.t, wave_length, sample_rate, freq_bins,
							   AMPLITUDE_SPECTRUM_FLAT, base_amplitude,
							   noise_variance);
				/* The binary file output is grouped into chunks for each
				 * channel, rather than storing each channel contiguously. This
				 * drastically improves performance when filtering there data,
				 * as that is done per chunk. */
				for (unsigned int j = 0; j < n_chunks; j++) {
					size_t element_start_index =
						(j * n_channels * chunk_size) + (i * chunk_size);
					std::streampos byte_offset =
						element_start_index * sizeof(num_t);
					/* Seek to the offset */
					this->stream_handle.seekp(byte_offset, std::ios::beg);
					/* Write the chunk to file */
					this->stream_handle.write(
						(const char *)&x.x[j * chunk_size],
						chunk_size * sizeof(num_t));
				}
			}
			/* Close the file */
			this->close_stream();
			return 0;
		}
	}
	/* Constructor */
	dsp_bin_gen(std::string _filename, unsigned int _chunk_size,
				unsigned int _n_chunks, unsigned int _freq_bins,
				unsigned int _n_channels, num_t _base_amplitude,
				num_t _noise_variance, unsigned int _sample_rate)
		: dsp_bin_base(_filename), chunk_size(_chunk_size), n_chunks(_n_chunks),
		  freq_bins(_freq_bins), n_channels(_n_channels),
		  base_amplitude(_base_amplitude), noise_variance(_noise_variance),
		  sample_rate(_sample_rate) {};
};

template <typename num_t> class dsp_bin_write : dsp_bin_base {
  public:
	/* Expose the base class methods */
	using dsp_bin_base::close_stream;
	bool open_stream() {
		return this->open_stream_write();
	} /* Access setting and function */
	unsigned int chunk_size;
	bool write_chunk(const std::vector<num_t> &chunk_data) {
		return (bool)this->stream_handle.write(
			reinterpret_cast<const char *>(chunk_data.data()),
			chunk_size * sizeof(num_t));
	}
	/* Constructor */
	dsp_bin_write(std::string _filename, unsigned int _chunk_size)
		: dsp_bin_base(_filename), chunk_size(_chunk_size) {}
};

template <typename num_t> class dsp_bin_read : dsp_bin_base {
  public:
	/* Expose the base class methods */
	using dsp_bin_base::close_stream;
	bool open_stream() { return this->open_stream_read(); }
	/* Access setting and function */
	unsigned int chunk_size;
	bool read_chunk(std::vector<num_t> &chunk_data) {
		return (bool)this->stream_handle.read(
			reinterpret_cast<char *>(chunk_data.data()),
			chunk_size * sizeof(num_t));
	}
	/* Constructor */
	dsp_bin_read(std::string _filename, unsigned int _chunk_size)
		: dsp_bin_base(_filename), chunk_size(_chunk_size) {}
};

#endif /* CPP_DATA_IO_H */

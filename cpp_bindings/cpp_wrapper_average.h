#ifndef DSP_TOOLBOX_CPP_WRAPPER_AVERAGE_H
#define DSP_TOOLBOX_CPP_WRAPPER_AVERAGE_H

/* C++ wrapper for averaging (average, power_average, moving_avg) and
 * statistics (rms, variance, std_dev, minimum, maximum, zero_crossings) */

#include "cpp_traits_average.h"

#include <cstdint>
#include <stdexcept>
#include <vector>

/* Mean of the samples, 0 if there are none */
template <typename num_t> num_t average(const std::vector<num_t> &data_in) {
	return avg_traits<num_t>::average(data_in.data(),
									  (uint32_t)data_in.size());
}
/* Pointer overload, for averaging part of a larger buffer without copying */
template <typename num_t> num_t average(const num_t *data_in, uint32_t length) {
	return avg_traits<num_t>::average(data_in, length);
}
/* Mean of the squares of the samples, 0 if there are none. int64_t for the
 * integer types */
template <typename num_t>
typename avg_traits<num_t>::power_type
power_average(const std::vector<num_t> &data_in) {
	return avg_traits<num_t>::power_average(data_in.data(),
											(uint32_t)data_in.size());
}
/* Pointer overload, for averaging part of a larger buffer without copying */
template <typename num_t>
typename avg_traits<num_t>::power_type power_average(const num_t *data_in,
													 uint32_t length) {
	return avg_traits<num_t>::power_average(data_in, length);
}

/* Root mean square of the samples, 0 if there are none. The integer types
 * are rounded to the nearest value. Not generated for float and double with
 * --no-stdlib */
template <typename num_t> num_t rms(const num_t *data_in, uint32_t length) {
	static_assert(avg_traits<num_t>::root_stats,
				  "rms needs the maths library, not generated with --no-stdlib");
	return avg_traits<num_t>::rms(data_in, length);
}
template <typename num_t> num_t rms(const std::vector<num_t> &data_in) {
	return rms(data_in.data(), (uint32_t)data_in.size());
}
/* Population variance of the samples (as numpy.var), 0 if there are none.
 * int64_t for the integer types, rounded to the nearest value */
template <typename num_t>
typename avg_traits<num_t>::power_type variance(const num_t *data_in,
												uint32_t length) {
	return avg_traits<num_t>::variance(data_in, length);
}
template <typename num_t>
typename avg_traits<num_t>::power_type
variance(const std::vector<num_t> &data_in) {
	return variance(data_in.data(), (uint32_t)data_in.size());
}
/* Population standard deviation of the samples, 0 if there are none. The
 * integer types are rounded to the nearest value. Not generated for float and
 * double with --no-stdlib */
template <typename num_t>
num_t std_dev(const num_t *data_in, uint32_t length) {
	static_assert(avg_traits<num_t>::root_stats,
				  "std_dev needs the maths library, not generated with "
				  "--no-stdlib");
	return avg_traits<num_t>::std_dev(data_in, length);
}
template <typename num_t> num_t std_dev(const std::vector<num_t> &data_in) {
	return std_dev(data_in.data(), (uint32_t)data_in.size());
}
/* Smallest and largest of the samples, 0 if there are none. index, if not
 * null, is set to the index of the first occurrence. NaNs are skipped,
 * unless every sample is NaN */
template <typename num_t>
num_t minimum(const num_t *data_in, uint32_t length,
			  uint32_t *index = nullptr) {
	return avg_traits<num_t>::minimum(data_in, length, index);
}
template <typename num_t>
num_t minimum(const std::vector<num_t> &data_in, uint32_t *index = nullptr) {
	return minimum(data_in.data(), (uint32_t)data_in.size(), index);
}
template <typename num_t>
num_t maximum(const num_t *data_in, uint32_t length,
			  uint32_t *index = nullptr) {
	return avg_traits<num_t>::maximum(data_in, length, index);
}
template <typename num_t>
num_t maximum(const std::vector<num_t> &data_in, uint32_t *index = nullptr) {
	return maximum(data_in.data(), (uint32_t)data_in.size(), index);
}
/* Number of sign changes between consecutive samples, with 0 counted as
 * positive */
template <typename num_t>
uint32_t zero_crossings(const num_t *data_in, uint32_t length) {
	return avg_traits<num_t>::zero_crossings(data_in, length);
}
template <typename num_t>
uint32_t zero_crossings(const std::vector<num_t> &data_in) {
	return zero_crossings(data_in.data(), (uint32_t)data_in.size());
}

/* Moving average wrapper class */
template <typename num_t> class moving_avg {
  private:
	using traits = avg_traits<num_t>;
	using avg_t = typename traits::struct_type;

	avg_t avg_struct;
	/* The window, when it isn't provided by the caller */
	std::vector<num_t> window;

	static uint32_t check_length(uint32_t length) {
		if (length == 0) {
			throw std::invalid_argument(
				"Error: Moving average window length must be at least 1.");
		}
		return length;
	}

  public:
	/* Allow copying of the class, this is useful when needing a multitude of
	 * the same moving average for multi-channel averaging. Copies have the
	 * same window length, with their own empty window */
	moving_avg(const moving_avg &other)
		: window(other.avg_struct.length, (num_t)0) {
		traits::init(&avg_struct, window.data(), other.avg_struct.length);
	};
	moving_avg &operator=(const moving_avg &other) {
		if (this != &other) {
			window.assign(other.avg_struct.length, (num_t)0);
			traits::init(&avg_struct, window.data(), other.avg_struct.length);
		}
		return *this;
	};
	/* Set move usage rules, moving the std::vector keeps its data in place so
	 * the struct's pointer to it stays valid */
	moving_avg(moving_avg &&) noexcept = default;
	moving_avg &operator=(moving_avg &&) noexcept = default;
	/* Constructors */
	/* Creates a window of length samples, throws std::invalid_argument if
	 * length is 0 */
	moving_avg(uint32_t length) : window(check_length(length), (num_t)0) {
		traits::init(&avg_struct, window.data(), length);
	};
	/* Uses the caller's buffer of length samples as the window, which must
	 * outlive the object */
	moving_avg(num_t *_buff, uint32_t length) {
		traits::init(&avg_struct, _buff, check_length(length));
	};
	/* Adds a sample to the window and returns the new average. Until the
	 * window is full, this is the average of the samples received so far */
	num_t moving_average(num_t new_samp) {
		return traits::moving_average(&avg_struct, new_samp);
	};
	/* Averages each sample of data_in into data_out, which may be the same
	 * vector */
	void moving_average(const std::vector<num_t> &data_in,
						std::vector<num_t> &data_out, uint32_t length) {
		traits::moving_average_block(&avg_struct, data_in.data(),
									 data_out.data(), length);
	};
	/* Pointer overload, allows averaging at an offset within a larger buffer
	 * without copying the data into a separate std::vector */
	void moving_average(const num_t *data_in, num_t *data_out,
						uint32_t length) {
		traits::moving_average_block(&avg_struct, data_in, data_out, length);
	};
	/* Empties the window */
	void reset() {
		traits::init(&avg_struct, avg_struct.buff, avg_struct.length);
	};
	/* Getter methods for the window length and the samples in the window */
	uint32_t get_length() const { return avg_struct.length; }
	uint32_t get_count() const { return avg_struct.count; }
};

#endif /* DSP_TOOLBOX_CPP_WRAPPER_AVERAGE_H */

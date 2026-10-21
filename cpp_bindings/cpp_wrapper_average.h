#ifndef DSP_TOOLBOX_CPP_WRAPPER_AVERAGE_H
#define DSP_TOOLBOX_CPP_WRAPPER_AVERAGE_H

/* C++ wrapper for averaging (average, power_average, moving_avg) */

#include "../average/avg_double.h"
#include "../average/avg_float.h"
#include "../average/avg_int32.h"
#include "../average/avg_int64.h"

#include <cstdint>
#include <stdexcept>
#include <vector>

/* Map the numerical type (float/double/int32_t/int64_t) to struct and
 * function types for averaging */
template <typename num_t> struct avg_traits;
/* Float Specialization */
template <> struct avg_traits<float> {
	using struct_type = moving_avg_float_t;
	using power_type = float;

	static inline float average(const float *data_in, uint32_t len) {
		return average_float(data_in, len);
	}
	static inline power_type power_average(const float *data_in, uint32_t len) {
		return power_average_float(data_in, len);
	}
	static inline void init(struct_type *avg, float *buff, uint32_t len) {
		moving_avg_init_float(avg, buff, len);
	}
	static inline float moving_average(struct_type *avg, float new_samp) {
		return moving_average_float(avg, new_samp);
	}
	static inline void moving_average_block(struct_type *avg,
											const float *data_in,
											float *data_out, uint32_t len) {
		moving_average_block_float(avg, data_in, data_out, len);
	}
};
/* Double Specialization */
template <> struct avg_traits<double> {
	using struct_type = moving_avg_double_t;
	using power_type = double;

	static inline double average(const double *data_in, uint32_t len) {
		return average_double(data_in, len);
	}
	static inline power_type power_average(const double *data_in, uint32_t len) {
		return power_average_double(data_in, len);
	}
	static inline void init(struct_type *avg, double *buff, uint32_t len) {
		moving_avg_init_double(avg, buff, len);
	}
	static inline double moving_average(struct_type *avg, double new_samp) {
		return moving_average_double(avg, new_samp);
	}
	static inline void moving_average_block(struct_type *avg,
											const double *data_in,
											double *data_out, uint32_t len) {
		moving_average_block_double(avg, data_in, data_out, len);
	}
};
/* Int32 Specialization */
template <> struct avg_traits<int32_t> {
	using struct_type = moving_avg_int32_t;
	/* The power is returned as int64_t, as the squares don't fit */
	using power_type = int64_t;

	static inline int32_t average(const int32_t *data_in, uint32_t len) {
		return average_int32(data_in, len);
	}
	static inline power_type power_average(const int32_t *data_in, uint32_t len) {
		return power_average_int32(data_in, len);
	}
	static inline void init(struct_type *avg, int32_t *buff, uint32_t len) {
		moving_avg_init_int32(avg, buff, len);
	}
	static inline int32_t moving_average(struct_type *avg, int32_t new_samp) {
		return moving_average_int32(avg, new_samp);
	}
	static inline void moving_average_block(struct_type *avg,
											const int32_t *data_in,
											int32_t *data_out, uint32_t len) {
		moving_average_block_int32(avg, data_in, data_out, len);
	}
};
/* Int64 Specialization */
template <> struct avg_traits<int64_t> {
	using struct_type = moving_avg_int64_t;
	using power_type = int64_t;

	static inline int64_t average(const int64_t *data_in, uint32_t len) {
		return average_int64(data_in, len);
	}
	static inline power_type power_average(const int64_t *data_in, uint32_t len) {
		return power_average_int64(data_in, len);
	}
	static inline void init(struct_type *avg, int64_t *buff, uint32_t len) {
		moving_avg_init_int64(avg, buff, len);
	}
	static inline int64_t moving_average(struct_type *avg, int64_t new_samp) {
		return moving_average_int64(avg, new_samp);
	}
	static inline void moving_average_block(struct_type *avg,
											const int64_t *data_in,
											int64_t *data_out, uint32_t len) {
		moving_average_block_int64(avg, data_in, data_out, len);
	}
};

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

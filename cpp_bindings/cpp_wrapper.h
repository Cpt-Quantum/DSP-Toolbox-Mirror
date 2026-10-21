#ifndef DSP_TOOLBOX_CPP_WRAPPER_H
#define DSP_TOOLBOX_CPP_WRAPPER_H

#include "../average/avg_double.h"
#include "../average/avg_float.h"
#include "../average/avg_int32.h"
#include "../average/avg_int64.h"
#include "../cjson/cJSON.h"
#include "../cjson/cJSON_helpers.h"
#include "../filter/fir_double.h"
#include "../filter/fir_float.h"
#include "../filter/fir_int32.h"
#include "../filter/fir_int64.h"
#include "../filter/iir_double.h"
#include "../filter/iir_float.h"
#include "../filter/iir_int32.h"
#include "../filter/iir_int64.h"
#include "../filter/json_to_filt_double.h"
#include "../filter/json_to_filt_float.h"
#include "../filter/json_to_filt_int32.h"
#include "../filter/json_to_filt_int64.h"
#include "../inc/spectrum_gen_double.h"
#include "../inc/spectrum_gen_float.h"
#include "../inc/spectrum_gen_int32.h"
#include "../inc/spectrum_gen_int64.h"
#include "../inc/spectrum_gen_shared.h"
#include "../inc/waveform_gen_double.h"
#include "../inc/waveform_gen_float.h"
#include "../inc/waveform_gen_int32.h"
#include "../inc/waveform_gen_int64.h"
#include "../inc/waveform_gen_shared.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

/* Map the numerical type (float/double/int32_t) to struct and function types
 * for waveform creation */
template <typename num_t> struct waveform_traits;
/* Float Specialization */
template <> struct waveform_traits<float> {
	using props_type = wave_properties_float_t;
	/* Floating point type of the times and wave properties */
	using math_type = wave_math_float_t;

	static inline void add_superposition(props_type props, math_type *t, float *y,
										 std::size_t len) {
		waveform_add_superposition_float(props, t, y, len);
	}
	static inline void gen_spectrum(math_type *t, float *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, float base_amplitude, float noise_variance,
									uint32_t seed) {
		generate_spectrum_waveform_float(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance, seed);
	}
	static inline void scale(float *x, float factor, uint32_t len) {
		scale_waveform_float(x, factor, len);
	}
	static inline void divide(float *x, float factor, uint32_t len) {
		divide_waveform_float(x, factor, len);
	}
	static inline void mask(float *x, uint32_t bit_width, uint32_t len) {
		mask_waveform_float(x, bit_width, len);
	}
};
/* Double Specialization */
template <> struct waveform_traits<double> {
	using props_type = wave_properties_double_t;
	/* Floating point type of the times and wave properties */
	using math_type = wave_math_double_t;

	static inline void add_superposition(props_type props, math_type *t, double *y,
										 std::size_t len) {
		waveform_add_superposition_double(props, t, y, len);
	}
	static inline void gen_spectrum(math_type *t, double *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, double base_amplitude, double noise_variance,
									uint32_t seed) {
		generate_spectrum_waveform_double(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance, seed);
	}
	static inline void scale(double *x, double factor, uint32_t len) {
		scale_waveform_double(x, factor, len);
	}
	static inline void divide(double *x, double factor, uint32_t len) {
		divide_waveform_double(x, factor, len);
	}
	static inline void mask(double *x, uint32_t bit_width, uint32_t len) {
		mask_waveform_double(x, bit_width, len);
	}
};
/* Int32 Specialization */
template <> struct waveform_traits<int32_t> {
	using props_type = wave_properties_int32_t;
	/* Floating point type of the times and wave properties */
	using math_type = wave_math_int32_t;

	static inline void add_superposition(props_type props, math_type *t,
										 int32_t *y, std::size_t len) {
		waveform_add_superposition_int32(props, t, y, len);
	}
	static inline void gen_spectrum(math_type *t, int32_t *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, int32_t base_amplitude, int32_t noise_variance,
									uint32_t seed) {
		generate_spectrum_waveform_int32(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance, seed);
	}
	static inline void scale(int32_t *x, int32_t factor, uint32_t len) {
		scale_waveform_int32(x, factor, len);
	}
	static inline void divide(int32_t *x, int32_t factor, uint32_t len) {
		divide_waveform_int32(x, factor, len);
	}
	static inline void mask(int32_t *x, uint32_t bit_width, uint32_t len) {
		mask_waveform_int32(x, bit_width, len);
	}
};
/* Int64 Specialization */
template <> struct waveform_traits<int64_t> {
	using props_type = wave_properties_int64_t;
	/* Floating point type of the times and wave properties */
	using math_type = wave_math_int64_t;

	static inline void add_superposition(props_type props, math_type *t,
										 int64_t *y, std::size_t len) {
		waveform_add_superposition_int64(props, t, y, len);
	}
	static inline void gen_spectrum(math_type *t, int64_t *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, int64_t base_amplitude, int64_t noise_variance,
									uint32_t seed) {
		generate_spectrum_waveform_int64(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance, seed);
	}
	static inline void scale(int64_t *x, int64_t factor, uint32_t len) {
		scale_waveform_int64(x, factor, len);
	}
	static inline void divide(int64_t *x, int64_t factor, uint32_t len) {
		divide_waveform_int64(x, factor, len);
	}
	static inline void mask(int64_t *x, uint32_t bit_width, uint32_t len) {
		mask_waveform_int64(x, bit_width, len);
	}
};
/* Time series for the waveforms of num_t. The times are in the floating
 * point type the waveforms are generated in (waveform_traits::math_type), so
 * they aren't truncated for the integer types */
template <typename num_t> class waveform_t {
  private:
	using math_t = typename waveform_traits<num_t>::math_type;

	unsigned int sample_rate;
	unsigned int wave_length;

  public:
	std::vector<math_t> t;
	waveform_t(unsigned int _sample_rate, unsigned int _wave_length) {
		/* Set the internal variables */
		sample_rate = _sample_rate;
		wave_length = _wave_length;
		/* Create the time array and then calculate the time values */
		t.reserve(_wave_length);
		for (unsigned int i = 0; i < _wave_length; i++) {
			t.push_back(((math_t)i) / _sample_rate);
		}
	};
};
/* Waveform creation/storage wrapper class */
template <typename num_t> class waveform_x {
  private:
	using traits = waveform_traits<num_t>;
	using props_t = typename traits::props_type;
	using math_t = typename traits::math_type;

	unsigned int wave_length;
	std::vector<props_t> wave_properties;

  public:
	std::vector<num_t> x;

	/* Constructors */
	waveform_x(unsigned int _wave_length) : wave_length(_wave_length) {
		x.resize(_wave_length);
	};
	waveform_x(unsigned int _wave_length, std::vector<props_t> _wave_properties)
		: wave_length(_wave_length),
		  wave_properties(std::move(_wave_properties)) {
		x.resize(_wave_length);
	};
	/* Methods */
	/* duty_cycle, sweep_rate and delay are only used by some functions, see
	 * wave_properties_*_t */
	void add_wave_properties(math_t amplitude, math_t offset, math_t frequency,
							 math_t phase, WAVEFORM_FUNC_E function_type,
							 math_t duty_cycle = 0, math_t sweep_rate = 0,
							 math_t delay = 0) {
		wave_properties.push_back({amplitude, offset, frequency, phase,
								   function_type, duty_cycle, sweep_rate,
								   delay});
	};
	void gen_waveform(std::vector<math_t> &t) {
		for (const auto &prop : wave_properties) {
			traits::add_superposition(prop, t.data(), x.data(), wave_length);
		}
	};
	void gen_spectrum(std::vector<math_t> &t, std::size_t length, uint32_t f_s,
					  uint32_t num_steps, AMPLITUDE_SPECTRUM_E profile, num_t base_amplitude, num_t noise_variance,
					  uint32_t seed) {
		traits::gen_spectrum(t.data(), x.data(), length, f_s, num_steps,
							 profile, base_amplitude, noise_variance, seed);
	};
	void scale(num_t factor) { traits::scale(x.data(), factor, wave_length); }
	void divide(num_t factor) { traits::divide(x.data(), factor, wave_length); }
	void mask(uint32_t bit_width) { traits::mask(x.data(), bit_width, wave_length); }
};

/* Map the numerical type (float/double/int32_t) to struct and function types
 * for averaging */
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

/* Wrapper class around the JSON parsing functions */
class json_filter_parse {
  private:
	struct cjson_deleter {
		void operator()(cJSON *ptr) const {
			if (ptr)
				cJSON_Delete(ptr);
		}
	};
	std::unique_ptr<cJSON, cjson_deleter> root = nullptr;

	/* Function called in the class constructor, this allows the constructor to
	 * throw an exception if the something in here fails */
	inline bool open_file(const std::string &filename) {
		/* Parse the file into the cJSON object */
		char *json_string = cJSON_read_file(filename.data());
		if (json_string == nullptr) {
			return false;
		}
		root.reset(cJSON_Parse(json_string));
		free(json_string);
		return (root != nullptr);
	}

  public:
	/* Set usage rules */
	json_filter_parse(const json_filter_parse &) = delete;
	json_filter_parse &operator=(const json_filter_parse &) = delete;
	/* Set move rules */
	json_filter_parse(json_filter_parse &&) noexcept = default;
	json_filter_parse &operator=(json_filter_parse &&) noexcept = default;
	/* Constuctor/destructor and deinit functions */
	json_filter_parse(const std::string &filename) {
		if (open_file(filename) == false) {
			throw std::runtime_error("Error: Failed to open JSON file.");
		}
	};
	/* Function to get the root cJSON object */
	cJSON *get_root() const {
		if (root == nullptr) {
			throw std::runtime_error(
				"Error: cJSON object not initialised (NULL).");
		} else {
			return root.get();
		}
	}
	/* Function to get a subnode */
	cJSON *get_node(const std::string &node_string) const {
		return cJSON_GetObjectItem(root.get(), node_string.data());
	}
};

/* Map the numerical type (float/double/int32_t) to struct and function types
 * for FIR filters */
template <typename num_t> struct fir_traits;
/* Float Specialization */
template <> struct fir_traits<float> {
	using struct_type = fir_float_t;

	static inline void filter(float *data_in, float *data_out,
							  struct_type *filt, uint32_t len) {
		fir_filter_float(data_in, data_out, filt, len);
	}
	static inline uint32_t decimate(float *data_in, float *data_out,
									struct_type *filt, uint32_t len,
									uint8_t rate) {
		return fir_decimate_float(data_in, data_out, filt, len, rate);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_fir_from_json_float(filt, node);
	}
	static inline void free(struct_type *filt) { free_fir_float(filt); }
};
/* Double Specialization */
template <> struct fir_traits<double> {
	using struct_type = fir_double_t;

	static inline void filter(double *data_in, double *data_out,
							  struct_type *filt, uint32_t len) {
		fir_filter_double(data_in, data_out, filt, len);
	}
	static inline uint32_t decimate(double *data_in, double *data_out,
									struct_type *filt, uint32_t len,
									uint8_t rate) {
		return fir_decimate_double(data_in, data_out, filt, len, rate);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_fir_from_json_double(filt, node);
	}
	static inline void free(struct_type *filt) { free_fir_double(filt); }
};
/* Int32 Specialization */
template <> struct fir_traits<int32_t> {
	using struct_type = fir_int32_t;

	static inline void filter(int32_t *data_in, int32_t *data_out,
							  struct_type *filt, uint32_t len) {
		fir_filter_int32(data_in, data_out, filt, len);
	}
	static inline uint32_t decimate(int32_t *data_in, int32_t *data_out,
									struct_type *filt, uint32_t len,
									uint8_t rate) {
		return fir_decimate_int32(data_in, data_out, filt, len, rate);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_fir_from_json_int32(filt, node);
	}
	static inline void free(struct_type *filt) { free_fir_int32(filt); }
};
/* Int64 Specialization */
template <> struct fir_traits<int64_t> {
	using struct_type = fir_int64_t;

	static inline void filter(int64_t *data_in, int64_t *data_out,
							  struct_type *filt, uint32_t len) {
		fir_filter_int64(data_in, data_out, filt, len);
	}
	static inline uint32_t decimate(int64_t *data_in, int64_t *data_out,
									struct_type *filt, uint32_t len,
									uint8_t rate) {
		return fir_decimate_int64(data_in, data_out, filt, len, rate);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_fir_from_json_int64(filt, node);
	}
	static inline void free(struct_type *filt) { free_fir_int64(filt); }
};
/* FIR wrapper class */
template <typename num_t> class fir {
  private:
	using traits = fir_traits<num_t>;
	using fir_t = typename traits::struct_type;

	/* Custom deleter object */
	struct fir_deleter {
		bool cJSON_used = false;

		void operator()(fir_t *ptr) const {
			if (ptr) {
				/* If the C library allocated internal arrays, free them */
				if (cJSON_used) {
					traits::free(ptr);
				}
				/* Always free the C++ allocated struct itself */
				delete ptr;
			}
		}
	};

	std::unique_ptr<fir_t, fir_deleter> filt_struct;

	/* Create a clone of the input buffers to support multi-channel filtering
	 * from the same shared filter specs */
	std::vector<num_t> in_buff_clone;

  public:
	/* Allow copying of the class, this is useful when needing a multitude of
	 * the same filter for multi-channel filtering. */
	fir(const fir &other) : filt_struct(new fir_t(), fir_deleter{false}) {
		/* Initialise a new input history buffer in the std::vector, sized for
		 * the number of taps */
		in_buff_clone.assign(FILTER_STATE_SIZE(other.filt_struct->taps),
							 (num_t)0);
		/* Copy the struct to share the coefficients, taps and (for the
		 * integer types) the fixed point format */
		*filt_struct = *other.filt_struct;
		/* Point the struct at the new input buffer and reset the decimation
		 * phase to 0 */
		filt_struct->in_buff = in_buff_clone.data();
		filt_struct->decimation_phase = 0;
	};
	fir &operator=(const fir &other) {
		if (this != &other) {
			/* Create the object and initialise the input buffer std::vector
			 * element
			 */
			filt_struct = std::unique_ptr<fir_t, fir_deleter>(
				new fir_t(), fir_deleter{false});
			in_buff_clone.assign(FILTER_STATE_SIZE(other.filt_struct->taps),
								 (num_t)0);
			/* Copy the struct to share the coefficients, taps and (for the
			 * integer types) the fixed point format */
			*filt_struct = *other.filt_struct;
			/* Point the struct at the new input buffer and reset the decimation
			 * phase to 0 */
			filt_struct->in_buff = in_buff_clone.data();
			filt_struct->decimation_phase = 0;
		}
		return *this;
	};
	/* Set move usage rules */
	fir(fir &&) noexcept = default;
	fir &operator=(fir &&) noexcept = default;
	/* Constructors */
	/* _in_buff must be zeroed and hold FILTER_STATE_SIZE(_taps) elements */
	fir(num_t *_b, num_t *_in_buff, uint16_t _taps)
		: filt_struct(new fir_t(), fir_deleter{false}) {
		filt_struct->b = _b;
		filt_struct->in_buff = _in_buff;
		filt_struct->taps = _taps;
		filt_struct->decimation_phase = 0;
	};
	/* Integer types only, for Q format coefficients with _frac_bits
	 * fractional bits */
	template <typename T = num_t,
			  typename = std::enable_if_t<std::is_integral<T>::value>>
	fir(num_t *_b, num_t *_in_buff, uint16_t _taps, uint8_t _frac_bits)
		: fir(_b, _in_buff, _taps) {
		filt_struct->frac_bits = _frac_bits;
	};
	fir(fir_t *_filt_struct) : filt_struct(_filt_struct, fir_deleter{false}) {};
	fir(cJSON *node) : filt_struct(new fir_t(), fir_deleter{true}) {
		traits::create(filt_struct.get(), node);
	};
	/* Wrappers for the FIR filter and decimation functions */
	void fir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out,
					uint32_t length) {
		traits::filter(data_in.data(), data_out.data(), filt_struct.get(),
					   length);
	};
	/* Pointer overload, allows filtering at an offset within a larger buffer
	 * without copying the data into a separate std::vector */
	void fir_filter(num_t *data_in, num_t *data_out, uint32_t length) {
		traits::filter(data_in, data_out, filt_struct.get(), length);
	};
	/* Returns the number of output samples written */
	uint32_t fir_decimate(std::vector<num_t> &data_in,
						  std::vector<num_t> &data_out, uint32_t length,
						  uint8_t decimation_rate) {
		return traits::decimate(data_in.data(), data_out.data(),
								filt_struct.get(), length, decimation_rate);
	};
	/* Pointer overload, allows filtering at an offset within a larger buffer
	 * without copying the data into a separate std::vector */
	uint32_t fir_decimate(num_t *data_in, num_t *data_out, uint32_t length,
						  uint8_t decimation_rate) {
		return traits::decimate(data_in, data_out, filt_struct.get(), length,
								decimation_rate);
	};
	/* Getter methods for coefficients and taps */
	std::vector<num_t> get_coefficients() const {
		return std::vector<num_t>(filt_struct->b,
								  filt_struct->b + filt_struct->taps);
	}
	uint16_t get_num_taps() const { return filt_struct->taps; }
	/* Integer types only */
	template <typename T = num_t,
			  typename = std::enable_if_t<std::is_integral<T>::value>>
	uint8_t get_frac_bits() const {
		return filt_struct->frac_bits;
	}
};

/* Map the numerical type (float/double/int32_t) to struct and function types
 * for IIR filters (direct form) */
template <typename num_t> struct iir_traits;
/* Float Specialization */
template <> struct iir_traits<float> {
	using struct_type = iir_float_t;

	static inline void filter(float *data_in, float *data_out,
							  struct_type *filt, uint32_t len) {
		iir_filter_float(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_iir_from_json_float(filt, node);
	}
	static inline void free(struct_type *filt) { free_iir_float(filt); }
};
/* Double Specialization */
template <> struct iir_traits<double> {
	using struct_type = iir_double_t;

	static inline void filter(double *data_in, double *data_out,
							  struct_type *filt, uint32_t len) {
		iir_filter_double(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_iir_from_json_double(filt, node);
	}
	static inline void free(struct_type *filt) { free_iir_double(filt); }
};
/* Int32 Specialization */
template <> struct iir_traits<int32_t> {
	using struct_type = iir_int32_t;

	static inline void filter(int32_t *data_in, int32_t *data_out,
							  struct_type *filt, uint32_t len) {
		iir_filter_int32(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_iir_from_json_int32(filt, node);
	}
	static inline void free(struct_type *filt) { free_iir_int32(filt); }
};
/* Int64 Specialization */
template <> struct iir_traits<int64_t> {
	using struct_type = iir_int64_t;

	static inline void filter(int64_t *data_in, int64_t *data_out,
							  struct_type *filt, uint32_t len) {
		iir_filter_int64(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_iir_from_json_int64(filt, node);
	}
	static inline void free(struct_type *filt) { free_iir_int64(filt); }
};
/* IIR (direct form) wrapper class */
template <typename num_t> class iir_ba {
  private:
	using traits = iir_traits<num_t>;
	using iir_t = typename traits::struct_type;

	/* Custom deleter object */
	struct iir_deleter {
		bool cJSON_used = false;

		void operator()(iir_t *ptr) const {
			if (ptr) {
				/* If the C library allocated internal arrays, free them */
				if (cJSON_used) {
					traits::free(ptr);
				}
				/* Always free the C++ allocated struct itself */
				delete ptr;
			}
		}
	};

	std::unique_ptr<iir_t, iir_deleter> filt_struct;

	/* Create a clone of the input and output buffers to support multi-channel
	 * filtering from the same shared filter specs */
	std::vector<num_t> in_buff_clone;
	std::vector<num_t> out_buff_clone;

  public:
	/* Allow copying of the class, this is useful when needing a multitude of
	 * the same filter for multi-channel filtering. */
	iir_ba(const iir_ba &other) : filt_struct(new iir_t(), iir_deleter{false}) {
		/* Initialise new input and output history buffers in the std::vectors,
		 * sized for the number of taps */
		in_buff_clone.assign(FILTER_STATE_SIZE(other.filt_struct->taps),
							 (num_t)0);
		out_buff_clone.assign(FILTER_STATE_SIZE(other.filt_struct->taps),
							  (num_t)0);
		/* Copy the struct to share the coefficients, taps and (for the
		 * integer types) the fixed point format */
		*filt_struct = *other.filt_struct;
		/* Point the struct at the new input and output buffers */
		filt_struct->in_buff = in_buff_clone.data();
		filt_struct->out_buff = out_buff_clone.data();
	}
	iir_ba &operator=(const iir_ba &other) {
		if (this != &other) {
			/* Create the new object to point to */
			filt_struct = std::unique_ptr<iir_t, iir_deleter>(
				new iir_t(), iir_deleter{false});
			/* Initialise new input and output history buffers in the
			 * std::vectors, sized for the number of taps */
			in_buff_clone.assign(FILTER_STATE_SIZE(other.filt_struct->taps),
								 (num_t)0);
			out_buff_clone.assign(FILTER_STATE_SIZE(other.filt_struct->taps),
								  (num_t)0);
			/* Copy the struct to share the coefficients, taps and (for the
			 * integer types) the fixed point format */
			*filt_struct = *other.filt_struct;
			/* Point the struct at the new input and output buffers */
			filt_struct->in_buff = in_buff_clone.data();
			filt_struct->out_buff = out_buff_clone.data();
		}
		return *this;
	}
	/* Set the move usage rules */
	iir_ba(iir_ba &&) noexcept = default;
	iir_ba &operator=(iir_ba &&) noexcept = default;
	/* Constructors */
	/* _in_buff and _out_buff must be zeroed and hold FILTER_STATE_SIZE(taps)
	 * elements each */
	iir_ba(num_t *_a, num_t *_b, num_t *_in_buff, num_t *_out_buff,
		   uint16_t taps)
		: filt_struct(new iir_t(), iir_deleter{false}) {
		filt_struct->a = _a;
		filt_struct->b = _b;
		filt_struct->in_buff = _in_buff;
		filt_struct->out_buff = _out_buff;
		filt_struct->taps = taps;
	};
	/* Integer types only, for Q format coefficients with _frac_bits
	 * fractional bits. _a[0] must be 1 << _frac_bits */
	template <typename T = num_t,
			  typename = std::enable_if_t<std::is_integral<T>::value>>
	iir_ba(num_t *_a, num_t *_b, num_t *_in_buff, num_t *_out_buff,
		   uint16_t taps, uint8_t _frac_bits)
		: iir_ba(_a, _b, _in_buff, _out_buff, taps) {
		filt_struct->frac_bits = _frac_bits;
	};
	iir_ba(iir_t *raw_filt_struct)
		: filt_struct(raw_filt_struct, iir_deleter{false}) {};
	iir_ba(cJSON *node) : filt_struct(new iir_t(), iir_deleter{true}) {
		traits::create(filt_struct.get(), node);
	};
	/* Wrap the IIR filter function */
	void iir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out,
					uint32_t length) {
		traits::filter(data_in.data(), data_out.data(), filt_struct.get(),
					   length);
	};
	/* Pointer overload, allows filtering at an offset within a larger buffer
	 * without copying the data into a separate std::vector */
	void iir_filter(num_t *data_in, num_t *data_out, uint32_t length) {
		traits::filter(data_in, data_out, filt_struct.get(), length);
	};
	/* Getter methods for coefficients and taps */
	std::vector<num_t> get_a_coefficients() const {
		return std::vector<num_t>(filt_struct->a,
								  filt_struct->a + filt_struct->taps);
	}
	std::vector<num_t> get_b_coefficients() const {
		return std::vector<num_t>(filt_struct->b,
								  filt_struct->b + filt_struct->taps);
	}
	uint16_t get_num_taps() const { return filt_struct->taps; }
	/* Integer types only */
	template <typename T = num_t,
			  typename = std::enable_if_t<std::is_integral<T>::value>>
	uint8_t get_frac_bits() const {
		return filt_struct->frac_bits;
	}
};

/* Map the numerical type (float/double/int32_t) to struct and function types
 * for IIR filters (SOS) */
template <typename num_t> struct sos_traits;
/* Float Specialization */
template <> struct sos_traits<float> {
	using struct_type = sos_filter_float_t;
	using biquad_type = biquad_section_float_t;

	static inline void filter(float *data_in, float *data_out,
							  struct_type *filt, uint32_t len) {
		sos_filter_float(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_sos_from_json_float(filt, node);
	}
	static inline void free(struct_type *filt) { free_sos_float(filt); }
};
/* Double Specialization */
template <> struct sos_traits<double> {
	using struct_type = sos_filter_double_t;
	using biquad_type = biquad_section_double_t;

	static inline void filter(double *data_in, double *data_out,
							  struct_type *filt, uint32_t len) {
		sos_filter_double(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_sos_from_json_double(filt, node);
	}
	static inline void free(struct_type *filt) { free_sos_double(filt); }
};
/* Int32 Specialization */
template <> struct sos_traits<int32_t> {
	using struct_type = sos_filter_int32_t;
	using biquad_type = biquad_section_int32_t;

	static inline void filter(int32_t *data_in, int32_t *data_out,
							  struct_type *filt, uint32_t len) {
		sos_filter_int32(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_sos_from_json_int32(filt, node);
	}
	static inline void free(struct_type *filt) { free_sos_int32(filt); }
};
/* Int64 Specialization */
template <> struct sos_traits<int64_t> {
	using struct_type = sos_filter_int64_t;
	using biquad_type = biquad_section_int64_t;

	static inline void filter(int64_t *data_in, int64_t *data_out,
							  struct_type *filt, uint32_t len) {
		sos_filter_int64(data_in, data_out, filt, len);
	}
	static inline void create(struct_type *filt, cJSON *node) {
		init_sos_from_json_int64(filt, node);
	}
	static inline void free(struct_type *filt) { free_sos_int64(filt); }
};
/* Biquad coefficients structure for SOS filters */
template <typename num_t> struct biquad_coefficients {
	std::vector<num_t> a;
	std::vector<num_t> b;
};

/* IIR (SOS) wrapper class */
template <typename num_t> class iir_sos {
  private:
	using traits = sos_traits<num_t>;
	using iir_t = typename traits::struct_type;
	using biquad_t = typename traits::biquad_type;

	/* Custom deleter object */
	struct iir_deleter {
		bool cJSON_used = false;

		void operator()(iir_t *ptr) const {
			if (ptr) {
				/* If the C library allocated internal arrays, free them */
				if (cJSON_used) {
					traits::free(ptr);
				}
				/* Always free the C++ allocated struct itself */
				delete ptr;
			}
		}
	};

	std::unique_ptr<iir_t, iir_deleter> filt_struct;
	/* Create a clone of the section order sections structures to support
	 * multi-channel filtering from the same shared filter specs */
	std::vector<biquad_t> sections_clone;

  public:
	/* Allow copying of the class, this is useful when needing a multitude of
	 * the same filter for multi-channel filtering. */
	iir_sos(const iir_sos &other)
		: filt_struct(new iir_t(), iir_deleter{false}) {
		/* Create the sections std::vector with the expected size */
		sections_clone.assign(other.filt_struct->biquad_section,
							  other.filt_struct->biquad_section +
								  other.filt_struct->num_sections);
		/* Copy the struct for the number of sections, gain and (for the
		 * integer types) the fixed point format */
		*filt_struct = *other.filt_struct;
		/* Point the struct at the copied sections */
		filt_struct->biquad_section = sections_clone.data();
	}
	iir_sos &operator=(const iir_sos &other) {
		if (this != &other) {
			/* Create the new object */
			filt_struct = std::unique_ptr<iir_t, iir_deleter>(
				new iir_t(), iir_deleter{false});
			/* Create the sections std::vector */
			sections_clone.assign(other.filt_struct->biquad_section,
								  other.filt_struct->biquad_section +
									  other.filt_struct->num_sections);
			/* Copy the struct for the number of sections, gain and (for the
			 * integer types) the fixed point format */
			*filt_struct = *other.filt_struct;
			/* Point the struct at the copied sections */
			filt_struct->biquad_section = sections_clone.data();
		}
		return *this;
	}
	/* Set the move usage rules */
	iir_sos(iir_sos &&) noexcept = default;
	iir_sos &operator=(iir_sos &&) noexcept = default;
	/* Constructors */
	iir_sos(biquad_t *_sections, uint16_t _n_sections, num_t _gain)
		: filt_struct(new iir_t(), iir_deleter{false}) {
		filt_struct->biquad_section = _sections;
		filt_struct->num_sections = _n_sections;
		filt_struct->gain = _gain;
	};
	/* Integer types only, for Q format section coefficients and gain with
	 * _frac_bits fractional bits. Each section's a[0] must be
	 * 1 << _frac_bits */
	template <typename T = num_t,
			  typename = std::enable_if_t<std::is_integral<T>::value>>
	iir_sos(biquad_t *_sections, uint16_t _n_sections, num_t _gain,
			uint8_t _frac_bits)
		: iir_sos(_sections, _n_sections, _gain) {
		filt_struct->frac_bits = _frac_bits;
	};
	iir_sos(iir_t *raw_filt_struct)
		: filt_struct(raw_filt_struct, iir_deleter{false}) {};

	iir_sos(cJSON *node) : filt_struct(new iir_t(), iir_deleter{true}) {
		traits::create(filt_struct.get(), node);
	};
	/* Wrap the IIR filter function */
	void iir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out,
					uint32_t length) {
		traits::filter(data_in.data(), data_out.data(), filt_struct.get(),
					   length);
	};
	/* Pointer overload, allows filtering at an offset within a larger buffer
	 * without copying the data into a separate std::vector */
	void iir_filter(num_t *data_in, num_t *data_out, uint32_t length) {
		traits::filter(data_in, data_out, filt_struct.get(), length);
	};
	/* Getter methods for coefficients, gain and sections */
	std::vector<biquad_coefficients<num_t>> get_sections() const {
		std::vector<biquad_coefficients<num_t>> sections;
		sections.reserve(filt_struct->num_sections);
		for (uint16_t i = 0; i < filt_struct->num_sections; ++i) {
			sections.push_back(
				{std::vector<num_t>(filt_struct->biquad_section[i].a,
									filt_struct->biquad_section[i].a + 3),
				 std::vector<num_t>(filt_struct->biquad_section[i].b,
									filt_struct->biquad_section[i].b + 3)});
		}
		return sections;
	}
	uint16_t get_num_sections() const { return filt_struct->num_sections; }
	num_t get_gain() const { return filt_struct->gain; }
	/* Integer types only */
	template <typename T = num_t,
			  typename = std::enable_if_t<std::is_integral<T>::value>>
	uint8_t get_frac_bits() const {
		return filt_struct->frac_bits;
	}
};

#endif /* DSP_TOOLBOX_CPP_WRAPPER_H */

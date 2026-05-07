#ifndef DSP_TOOLBOX_CPP_WRAPPER_H
#define DSP_TOOLBOX_CPP_WRAPPER_H

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
#include <vector>

template <typename num_t> class waveform_t {
  private:
	unsigned int sample_rate;
	unsigned int wave_length;

  public:
	std::vector<num_t> t;
	waveform_t(unsigned int _sample_rate, unsigned int _wave_length) {
		/* Set the internal variables */
		sample_rate = _sample_rate;
		wave_length = _wave_length;
		/* Create the time array and then calculate the time values */
		t.reserve(_wave_length);
		for (unsigned int i = 0; i < _wave_length; i++) {
			t.push_back(((num_t)i) / _sample_rate);
		}
	};
};

/* Map the numerical type (float/double/int32_t) to struct and function types
 * for waveform creation */
template <typename num_t> struct waveform_traits;
/* Float Specialization */
template <> struct waveform_traits<float> {
	using props_type = wave_properties_float_t;

	static inline void add_superposition(props_type props, float *t, float *y,
										 std::size_t len) {
		waveform_add_superposition_float(props, t, y, len);
	}
	static inline void gen_spectrum(float *t, float *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, float base_amplitude, float noise_variance) {
		generate_spectrum_waveform_float(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance);
	}
	static inline void scale(float *x, float factor, uint32_t len) {
		scale_waveform_float(x, factor, len);
	}
	static inline void divide(float *x, float factor, uint32_t len) {
		divide_waveform_float(x, factor, len);
	}
};
/* Double Specialization */
template <> struct waveform_traits<double> {
	using props_type = wave_properties_double_t;

	static inline void add_superposition(props_type props, double *t, double *y,
										 std::size_t len) {
		waveform_add_superposition_double(props, t, y, len);
	}
	static inline void gen_spectrum(double *t, double *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, double base_amplitude, double noise_variance) {
		generate_spectrum_waveform_double(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance);
	}
	static inline void scale(double *x, double factor, uint32_t len) {
		scale_waveform_double(x, factor, len);
	}
	static inline void divide(double *x, double factor, uint32_t len) {
		divide_waveform_double(x, factor, len);
	}
};
/* Int32 Specialization */
template <> struct waveform_traits<int32_t> {
	using props_type = wave_properties_int32_t;

	static inline void add_superposition(props_type props, int32_t *t,
										 int32_t *y, std::size_t len) {
		waveform_add_superposition_int32(props, t, y, len);
	}
	static inline void gen_spectrum(int32_t *t, int32_t *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, int32_t base_amplitude, int32_t noise_variance) {
		generate_spectrum_waveform_int32(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance);
	}
	static inline void scale(int32_t *x, int32_t factor, uint32_t len) {
		scale_waveform_int32(x, factor, len);
	}
	static inline void divide(int32_t *x, int32_t factor, uint32_t len) {
		divide_waveform_int32(x, factor, len);
	}
};
/* Int64 Specialization */
template <> struct waveform_traits<int64_t> {
	using props_type = wave_properties_int64_t;

	static inline void add_superposition(props_type props, int64_t *t,
										 int64_t *y, std::size_t len) {
		waveform_add_superposition_int64(props, t, y, len);
	}
	static inline void gen_spectrum(int64_t *t, int64_t *y, std::size_t len,
									uint32_t f_s, uint32_t num_steps,
									AMPLITUDE_SPECTRUM_E profile, int64_t base_amplitude, int64_t noise_variance) {
		generate_spectrum_waveform_int64(t, y, len, f_s, num_steps, profile, base_amplitude, noise_variance);
	}
	static inline void scale(int64_t *x, int64_t factor, uint32_t len) {
		scale_waveform_int64(x, factor, len);
	}
	static inline void divide(int64_t *x, int64_t factor, uint32_t len) {
		divide_waveform_int64(x, factor, len);
	}
};
/* Waveform creation/storage wrapper class */
template <typename num_t> class waveform_x {
  private:
	using traits = waveform_traits<num_t>;
	using props_t = typename traits::props_type;

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
	void add_wave_properties(num_t amplitude, num_t offset, num_t frequency,
							 num_t phase, WAVEFORM_FUNC_E function_type) {
		wave_properties.push_back(
			{amplitude, offset, frequency, phase, function_type});
	};
	void gen_waveform(std::vector<num_t> &t) {
		for (const auto &prop : wave_properties) {
			traits::add_superposition(prop, t.data(), x.data(), wave_length);
		}
	};
	void gen_spectrum(std::vector<num_t> &t, std::size_t length, uint32_t f_s,
					  uint32_t num_steps, AMPLITUDE_SPECTRUM_E profile, num_t base_amplitude, num_t noise_variance) {
		traits::gen_spectrum(t.data(), x.data(), length, f_s, num_steps,
							 profile, base_amplitude, noise_variance);
	};
	void scale(num_t factor) { traits::scale(x.data(), factor, wave_length); }
	void divide(num_t factor) { traits::divide(x.data(), factor, wave_length); }
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
	static inline void decimate(float *data_in, float *data_out,
								struct_type *filt, uint32_t len, uint8_t rate) {
		fir_decimate_float(data_in, data_out, filt, len, rate);
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
	static inline void decimate(double *data_in, double *data_out,
								struct_type *filt, uint32_t len, uint8_t rate) {
		fir_decimate_double(data_in, data_out, filt, len, rate);
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
	static inline void decimate(int32_t *data_in, int32_t *data_out,
								struct_type *filt, uint32_t len, uint8_t rate) {
		fir_decimate_int32(data_in, data_out, filt, len, rate);
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
	static inline void decimate(int64_t *data_in, int64_t *data_out,
								struct_type *filt, uint32_t len, uint8_t rate) {
		fir_decimate_int64(data_in, data_out, filt, len, rate);
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
		/* Initialise a new input buffer in the std::vector with the number of
		 * taps as the number of elements */
		in_buff_clone.assign(other.filt_struct->taps, (num_t)0);
		/* Get the pointer of the std::vector data and pass it to the struct's
		 * pointer */
		filt_struct->in_buff = in_buff_clone.data();
		/* Get the pointer to the shared coefficients */
		filt_struct->b = other.filt_struct->b;
		/* Set the number of taps in the struct member and reset the cycle count
		 * to 0 */
		filt_struct->taps = other.filt_struct->taps;
		filt_struct->cycle = 0;
	};
	fir &operator=(const fir &other) {
		if (this != &other) {
			/* Create the object and initialise the input buffer std::vector
			 * element
			 */
			filt_struct = std::unique_ptr<fir_t, fir_deleter>(
				new fir_t(), fir_deleter{false});
			in_buff_clone.assign(other.filt_struct->taps, (num_t)0);
			/* Set the struct pointer for the input buffer */
			filt_struct->in_buff = in_buff_clone.data();
			/* Get the pointer to the shared coefficients */
			filt_struct->b = other.filt_struct->b;
			/* Set the number of taps in the struct member and reset the cycle
			 * count to 0 */
			filt_struct->taps = other.filt_struct->taps;
			filt_struct->cycle = 0;
		}
		return *this;
	};
	/* Set move usage rules */
	fir(fir &&) noexcept = default;
	fir &operator=(fir &&) noexcept = default;
	/* Constructors */
	fir(num_t *_b, num_t *_in_buff, uint16_t _taps)
		: filt_struct(new fir_t(), fir_deleter{false}) {
		filt_struct->b = _b;
		filt_struct->in_buff = _in_buff;
		filt_struct->taps = _taps;
		filt_struct->cycle = 0;
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
	void fir_decimate(std::vector<num_t> &data_in, std::vector<num_t> &data_out,
					  uint32_t length, uint8_t decimation_rate) {
		traits::decimate(data_in.data(), data_out.data(), filt_struct.get(),
						 length, decimation_rate);
	};
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
		/* Initialise a new input buffer in the std::vector with the number of
		 * taps as the number of elements */
		in_buff_clone.assign(other.filt_struct->taps, (num_t)0);
		out_buff_clone.assign(other.filt_struct->taps, (num_t)0);
		/* Get the pointer of the std::vector data and pass it to the struct's
		 * pointer */
		filt_struct->in_buff = in_buff_clone.data();
		filt_struct->out_buff = out_buff_clone.data();
		/* Get the pointer to the shared coefficients */
		filt_struct->a = other.filt_struct->a;
		filt_struct->b = other.filt_struct->b;
		/* Set the number of taps in the struct member and reset the cycle count
		 * to 0 */
		filt_struct->taps = other.filt_struct->taps;
		filt_struct->cycle = 0;
	}
	iir_ba &operator=(const iir_ba &other) {
		if (this != &other) {
			/* Create the new object to point to */
			filt_struct = std::unique_ptr<iir_t, iir_deleter>(
				new iir_t(), iir_deleter{false});
			/* Initialise a new input buffer in the std::vector with the number
			 * of taps as the number of elements */
			in_buff_clone.assign(other.filt_struct->taps, (num_t)0);
			out_buff_clone.assign(other.filt_struct->taps, (num_t)0);
			/* Get the pointer of the std::vector data and pass it to the
			 * struct's pointer */
			filt_struct->in_buff = in_buff_clone.data();
			filt_struct->out_buff = out_buff_clone.data();
			/* Get the pointer to the shared coefficients */
			filt_struct->a = other.filt_struct->a;
			filt_struct->b = other.filt_struct->b;
			/* Set the number of taps in the struct member and reset the cycle
			 * count to 0 */
			filt_struct->taps = other.filt_struct->taps;
			filt_struct->cycle = 0;
		}
		return *this;
	}
	/* Set the move usage rules */
	iir_ba(iir_ba &&) noexcept = default;
	iir_ba &operator=(iir_ba &&) noexcept = default;
	/* Constructors */
	iir_ba(num_t *_a, num_t *_b, num_t *_in_buff, num_t *_out_buff,
		   uint16_t taps)
		: filt_struct(new iir_t(), iir_deleter{false}) {
		filt_struct->a = _a;
		filt_struct->b = _b;
		filt_struct->in_buff = _in_buff;
		filt_struct->out_buff = _out_buff;
		filt_struct->taps = taps;
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
		/* Pass the pointer to the std::vector raw data for the filter struct */
		filt_struct->biquad_section = sections_clone.data();
		/* Set the number of sections and gain from the reference object */
		filt_struct->num_sections = other.filt_struct->num_sections;
		filt_struct->gain = other.filt_struct->gain;
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
			/* Pass the pointer to the std::vector raw data for the filter
			 * struct */
			filt_struct->biquad_section = sections_clone.data();
			/* Set the number of sections and gain from the reference object */
			filt_struct->num_sections = other.filt_struct->num_sections;
			filt_struct->gain = other.filt_struct->gain;
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
};

#endif /* DSP_TOOLBOX_CPP_WRAPPER_H */

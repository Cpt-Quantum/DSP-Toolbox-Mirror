#ifndef DSP_TOOLBOX_CPP_WRAPPER_WAVEFORM_H
#define DSP_TOOLBOX_CPP_WRAPPER_WAVEFORM_H

/* C++ wrapper for waveform and spectrum generation (waveform_t, waveform_x)
 * and the streaming oscillators (nco) */

#include "../inc/nco_double.h"
#include "../inc/nco_float.h"
#include "../inc/nco_int32.h"
#include "../inc/nco_int64.h"
#include "../inc/nco_shared.h"
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

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

/* Map the numerical type (float/double/int32_t/int64_t) to struct and
 * function types for waveform creation */
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

/* Map the numerical type (float/double/int32_t/int64_t) to struct and
 * function types for the streaming oscillators */
template <typename num_t> struct nco_traits;
/* Float Specialization */
template <> struct nco_traits<float> {
	using struct_type = nco_float_t;
	using props_type = wave_properties_float_t;
	using complex_type = complex_float_t;
	/* Floating point type of the settings */
	using math_type = wave_math_float_t;

	static inline NCO_STATUS_E init(struct_type *nco, props_type props,
								uint32_t sample_rate) {
		return nco_init_float(nco, props, sample_rate);
	}
	static inline void set_frequency(struct_type *nco, math_type frequency) {
		nco_set_frequency_float(nco, frequency);
	}
	static inline void set_sweep(struct_type *nco, math_type sweep_rate) {
		nco_set_sweep_float(nco, sweep_rate);
	}
	static inline void set_phase(struct_type *nco, math_type phase) {
		nco_set_phase_float(nco, phase);
	}
	static inline float sample(struct_type *nco) {
		return nco_sample_float(nco);
	}
	static inline void generate(struct_type *nco, float *x, uint32_t len) {
		nco_generate_float(nco, x, len);
	}
	static inline void add(struct_type *nco, float *x, uint32_t len) {
		nco_add_float(nco, x, len);
	}
	static inline complex_type sample_complex(struct_type *nco) {
		return nco_sample_complex_float(nco);
	}
	static inline void generate_complex(struct_type *nco, complex_type *x,
										uint32_t len) {
		nco_generate_complex_float(nco, x, len);
	}
	static inline void add_complex(struct_type *nco, complex_type *x, uint32_t len) {
		nco_add_complex_float(nco, x, len);
	}
};
/* Double Specialization */
template <> struct nco_traits<double> {
	using struct_type = nco_double_t;
	using props_type = wave_properties_double_t;
	using complex_type = complex_double_t;
	/* Floating point type of the settings */
	using math_type = wave_math_double_t;

	static inline NCO_STATUS_E init(struct_type *nco, props_type props,
								uint32_t sample_rate) {
		return nco_init_double(nco, props, sample_rate);
	}
	static inline void set_frequency(struct_type *nco, math_type frequency) {
		nco_set_frequency_double(nco, frequency);
	}
	static inline void set_sweep(struct_type *nco, math_type sweep_rate) {
		nco_set_sweep_double(nco, sweep_rate);
	}
	static inline void set_phase(struct_type *nco, math_type phase) {
		nco_set_phase_double(nco, phase);
	}
	static inline double sample(struct_type *nco) {
		return nco_sample_double(nco);
	}
	static inline void generate(struct_type *nco, double *x, uint32_t len) {
		nco_generate_double(nco, x, len);
	}
	static inline void add(struct_type *nco, double *x, uint32_t len) {
		nco_add_double(nco, x, len);
	}
	static inline complex_type sample_complex(struct_type *nco) {
		return nco_sample_complex_double(nco);
	}
	static inline void generate_complex(struct_type *nco, complex_type *x,
										uint32_t len) {
		nco_generate_complex_double(nco, x, len);
	}
	static inline void add_complex(struct_type *nco, complex_type *x, uint32_t len) {
		nco_add_complex_double(nco, x, len);
	}
};
/* Int32 Specialization */
template <> struct nco_traits<int32_t> {
	using struct_type = nco_int32_t;
	using props_type = wave_properties_int32_t;
	using complex_type = complex_int32_t;
	/* Floating point type of the settings */
	using math_type = wave_math_int32_t;

	static inline NCO_STATUS_E init(struct_type *nco, props_type props,
								uint32_t sample_rate) {
		return nco_init_int32(nco, props, sample_rate);
	}
	static inline void set_frequency(struct_type *nco, math_type frequency) {
		nco_set_frequency_int32(nco, frequency);
	}
	static inline void set_sweep(struct_type *nco, math_type sweep_rate) {
		nco_set_sweep_int32(nco, sweep_rate);
	}
	static inline void set_phase(struct_type *nco, math_type phase) {
		nco_set_phase_int32(nco, phase);
	}
	static inline int32_t sample(struct_type *nco) {
		return nco_sample_int32(nco);
	}
	static inline void generate(struct_type *nco, int32_t *x, uint32_t len) {
		nco_generate_int32(nco, x, len);
	}
	static inline void add(struct_type *nco, int32_t *x, uint32_t len) {
		nco_add_int32(nco, x, len);
	}
	static inline complex_type sample_complex(struct_type *nco) {
		return nco_sample_complex_int32(nco);
	}
	static inline void generate_complex(struct_type *nco, complex_type *x,
										uint32_t len) {
		nco_generate_complex_int32(nco, x, len);
	}
	static inline void add_complex(struct_type *nco, complex_type *x, uint32_t len) {
		nco_add_complex_int32(nco, x, len);
	}
};
/* Int64 Specialization */
template <> struct nco_traits<int64_t> {
	using struct_type = nco_int64_t;
	using props_type = wave_properties_int64_t;
	using complex_type = complex_int64_t;
	/* Floating point type of the settings */
	using math_type = wave_math_int64_t;

	static inline NCO_STATUS_E init(struct_type *nco, props_type props,
								uint32_t sample_rate) {
		return nco_init_int64(nco, props, sample_rate);
	}
	static inline void set_frequency(struct_type *nco, math_type frequency) {
		nco_set_frequency_int64(nco, frequency);
	}
	static inline void set_sweep(struct_type *nco, math_type sweep_rate) {
		nco_set_sweep_int64(nco, sweep_rate);
	}
	static inline void set_phase(struct_type *nco, math_type phase) {
		nco_set_phase_int64(nco, phase);
	}
	static inline int64_t sample(struct_type *nco) {
		return nco_sample_int64(nco);
	}
	static inline void generate(struct_type *nco, int64_t *x, uint32_t len) {
		nco_generate_int64(nco, x, len);
	}
	static inline void add(struct_type *nco, int64_t *x, uint32_t len) {
		nco_add_int64(nco, x, len);
	}
	static inline complex_type sample_complex(struct_type *nco) {
		return nco_sample_complex_int64(nco);
	}
	static inline void generate_complex(struct_type *nco, complex_type *x,
										uint32_t len) {
		nco_generate_complex_int64(nco, x, len);
	}
	static inline void add_complex(struct_type *nco, complex_type *x, uint32_t len) {
		nco_add_complex_int64(nco, x, len);
	}
};

/* Streaming waveform generator, carrying on from where the last call left
 * off. See nco_init_*() in the C API reference for the supported functions.
 * Copies carry on independently from the same point */
template <typename num_t> class nco {
  private:
	using traits = nco_traits<num_t>;
	using nco_t = typename traits::struct_type;
	using props_t = typename traits::props_type;
	using complex_t = typename traits::complex_type;
	using math_t = typename traits::math_type;

	nco_t nco_struct;

	void init(props_t props, uint32_t sample_rate) {
		switch (traits::init(&nco_struct, props, sample_rate)) {
		case NCO_OK:
			return;
		case NCO_ERR_INVALID_SAMPLE_RATE:
			throw std::invalid_argument("nco sample rate is 0");
		case NCO_ERR_INVALID_FUNCTION:
			throw std::invalid_argument(
				"nco function isn't periodic (impulse or step)");
		default:
			throw std::invalid_argument("nco initialisation failed");
		}
	}

  public:
	/* Constructors, throwing std::invalid_argument for a sample rate of 0 or
	 * FUNCTION_IMPULSE/FUNCTION_STEP. duty_cycle is only used by the square
	 * wave, and sweep_rate by the chirp */
	nco(props_t props, uint32_t sample_rate) { init(props, sample_rate); }
	nco(math_t amplitude, math_t offset, math_t frequency, math_t phase,
		WAVEFORM_FUNC_E function_type, uint32_t sample_rate,
		math_t duty_cycle = 0, math_t sweep_rate = 0) {
		init({amplitude, offset, frequency, phase, function_type, duty_cycle,
			  sweep_rate, 0},
			 sample_rate);
	}

	/* Changes from the next sample, see nco_set_*() */
	void set_frequency(math_t frequency) {
		traits::set_frequency(&nco_struct, frequency);
	}
	void set_sweep(math_t sweep_rate) {
		traits::set_sweep(&nco_struct, sweep_rate);
	}
	void set_phase(math_t phase) { traits::set_phase(&nco_struct, phase); }

	/* The next sample */
	num_t sample() { return traits::sample(&nco_struct); }
	complex_t sample_complex() { return traits::sample_complex(&nco_struct); }

	/* Fills x with the next samples */
	void generate(std::vector<num_t> &x) {
		traits::generate(&nco_struct, x.data(), (uint32_t)x.size());
	}
	void generate(num_t *x, uint32_t length) {
		traits::generate(&nco_struct, x, length);
	}
	void generate_complex(std::vector<complex_t> &x) {
		traits::generate_complex(&nco_struct, x.data(), (uint32_t)x.size());
	}
	void generate_complex(complex_t *x, uint32_t length) {
		traits::generate_complex(&nco_struct, x, length);
	}

	/* Adds the next samples to x */
	void add(std::vector<num_t> &x) {
		traits::add(&nco_struct, x.data(), (uint32_t)x.size());
	}
	void add(num_t *x, uint32_t length) { traits::add(&nco_struct, x, length); }
	void add_complex(std::vector<complex_t> &x) {
		traits::add_complex(&nco_struct, x.data(), (uint32_t)x.size());
	}
	void add_complex(complex_t *x, uint32_t length) {
		traits::add_complex(&nco_struct, x, length);
	}
};

#endif /* DSP_TOOLBOX_CPP_WRAPPER_WAVEFORM_H */

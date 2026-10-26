#ifndef DSP_TOOLBOX_CPP_WRAPPER_FFT_H
#define DSP_TOOLBOX_CPP_WRAPPER_FFT_H

/* C++ wrapper for the FFT (fft, rfft and dft), float and double only */

#include "../fft/fft_double.h"
#include "../fft/fft_float.h"
#include "../fft/fft_shared.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

/* The generation options decide which plan initialisations exist, and the
 * generated headers only define these macros when they do. The float and
 * double FFTs are always generated with the same options:
 * - FFT_PLAN_SIZE_FLOAT: workspace plans, left out by --fft-table-only
 * - FFT_TABLE_MAX_N_FLOAT: table plans, added by --fft-twiddle-table */

/* Map the numerical type (float/double) to struct and function types for the
 * FFT */
template <typename num_t> struct fft_traits;
/* Float Specialization */
template <> struct fft_traits<float> {
	using plan_type = fft_plan_float_t;
	using rplan_type = rfft_plan_float_t;
	using complex_type = complex_float_t;

#ifdef FFT_PLAN_SIZE_FLOAT
	static inline size_t plan_size(uint32_t n) { return fft_plan_size_float(n); }
	static inline FFT_STATUS_E plan_init(plan_type *plan, uint32_t n, void *work,
										 size_t work_bytes) {
		return fft_plan_init_float(plan, n, work, work_bytes);
	}
	static inline FFT_STATUS_E rplan_init(rplan_type *plan, uint32_t n,
										  void *work, size_t work_bytes) {
		return rfft_plan_init_float(plan, n, work, work_bytes);
	}
	static inline void dft(const complex_type *data_in, complex_type *data_out,
						   uint32_t n) {
		dft_float(data_in, data_out, n);
	}
#endif
#ifdef FFT_TABLE_MAX_N_FLOAT
	static constexpr uint32_t table_max_n = FFT_TABLE_MAX_N_FLOAT;
	static inline FFT_STATUS_E plan_init_table(plan_type *plan, uint32_t n) {
		return fft_plan_init_table_float(plan, n);
	}
	static inline FFT_STATUS_E rplan_init_table(rplan_type *plan, uint32_t n) {
		return rfft_plan_init_table_float(plan, n);
	}
#endif
	static inline void forward(const plan_type *plan, complex_type *x) {
		fft_float(plan, x);
	}
	static inline void inverse(const plan_type *plan, complex_type *x) {
		ifft_float(plan, x);
	}
	static inline void rforward(const rplan_type *plan, float *x) {
		rfft_float(plan, x);
	}
	static inline void rinverse(const rplan_type *plan, float *x) {
		irfft_float(plan, x);
	}
};
/* Double Specialization */
template <> struct fft_traits<double> {
	using plan_type = fft_plan_double_t;
	using rplan_type = rfft_plan_double_t;
	using complex_type = complex_double_t;

#ifdef FFT_PLAN_SIZE_DOUBLE
	static inline size_t plan_size(uint32_t n) { return fft_plan_size_double(n); }
	static inline FFT_STATUS_E plan_init(plan_type *plan, uint32_t n, void *work,
										 size_t work_bytes) {
		return fft_plan_init_double(plan, n, work, work_bytes);
	}
	static inline FFT_STATUS_E rplan_init(rplan_type *plan, uint32_t n,
										  void *work, size_t work_bytes) {
		return rfft_plan_init_double(plan, n, work, work_bytes);
	}
	static inline void dft(const complex_type *data_in, complex_type *data_out,
						   uint32_t n) {
		dft_double(data_in, data_out, n);
	}
#endif
#ifdef FFT_TABLE_MAX_N_DOUBLE
	static constexpr uint32_t table_max_n = FFT_TABLE_MAX_N_DOUBLE;
	static inline FFT_STATUS_E plan_init_table(plan_type *plan, uint32_t n) {
		return fft_plan_init_table_double(plan, n);
	}
	static inline FFT_STATUS_E rplan_init_table(rplan_type *plan, uint32_t n) {
		return rfft_plan_init_table_double(plan, n);
	}
#endif
	static inline void forward(const plan_type *plan, complex_type *x) {
		fft_double(plan, x);
	}
	static inline void inverse(const plan_type *plan, complex_type *x) {
		ifft_double(plan, x);
	}
	static inline void rforward(const rplan_type *plan, double *x) {
		rfft_double(plan, x);
	}
	static inline void rinverse(const rplan_type *plan, double *x) {
		irfft_double(plan, x);
	}
};

/* Throws std::invalid_argument for a failed plan initialisation */
inline void fft_check_status(FFT_STATUS_E status) {
	switch (status) {
	case FFT_OK:
		return;
	case FFT_ERR_NULL_POINTER:
		throw std::invalid_argument("fft plan or workspace is NULL");
	case FFT_ERR_INVALID_SIZE:
		throw std::invalid_argument(
			"fft size isn't a supported power of two of at least 2");
	case FFT_ERR_WORK_TOO_SMALL:
		throw std::invalid_argument("fft workspace is too small");
	case FFT_ERR_WORK_MISALIGNED:
		throw std::invalid_argument("fft workspace is misaligned");
	default:
		throw std::invalid_argument("fft plan initialisation failed");
	}
}

/* The plan and twiddle storage shared by fft and rfft. The twiddles are only
 * read after initialisation, so copies share them, and a copy can be used on
 * another thread at the same time as the original */
template <typename num_t, typename plan_t> class fft_plan_base {
  protected:
	using traits = fft_traits<num_t>;
	using complex_t = typename traits::complex_type;

	plan_t plan;
	/* The workspace, when it is owned by the object. Empty for a table or a
	 * caller's workspace */
	std::shared_ptr<const std::vector<complex_t>> twiddles;

	fft_plan_base() = default;

#ifdef FFT_PLAN_SIZE_FLOAT
	/* Allocates the workspace and passes it to init(plan, n, work, bytes) */
	template <typename init_t> void init_owned(uint32_t n, init_t init) {
		size_t bytes = traits::plan_size(n);
		if (bytes == 0) {
			fft_check_status(FFT_ERR_INVALID_SIZE);
		}
		auto work = std::make_shared<std::vector<complex_t>>(
			bytes / sizeof(complex_t));
		fft_check_status(init(&plan, n, work->data(), bytes));
		twiddles = std::move(work);
	}
#endif

	void check_length(std::size_t length) const {
		if (length != plan.n) {
			throw std::invalid_argument("fft data length isn't the plan size");
		}
	}

  public:
	/* The transform size */
	uint32_t size() const { return plan.n; }
	/* The C plan, for calling the C functions directly */
	const plan_t &get_plan() const { return plan; }
};

/* Complex FFT and inverse FFT of n values, n a power of two of at least 2.
 * The forward transform is unscaled and the inverse is scaled by 1 / n, as
 * numpy.fft.fft and numpy.fft.ifft. Output is in natural order */
template <typename num_t>
class fft : public fft_plan_base<num_t, typename fft_traits<num_t>::plan_type> {
  private:
	using base = fft_plan_base<num_t, typename fft_traits<num_t>::plan_type>;
	using traits = typename base::traits;
	using complex_t = typename base::complex_t;
	using base::check_length;
	using base::plan;

	fft() = default;

  public:
	using complex_type = complex_t;

#ifdef FFT_PLAN_SIZE_FLOAT
	/* Plans n point transforms with a workspace owned by the object, throws
	 * std::invalid_argument if n is not a power of two of at least 2 */
	explicit fft(uint32_t n) { this->init_owned(n, traits::plan_init); }
	/* Uses the caller's workspace of work_bytes, which must outlive the
	 * object and its copies. Throws std::invalid_argument on any error from
	 * fft_plan_init_*() */
	fft(uint32_t n, void *work, size_t work_bytes) {
		fft_check_status(traits::plan_init(&plan, n, work, work_bytes));
	}
	/* Workspace bytes needed for an n point plan, 0 if n is invalid */
	static size_t workspace_size(uint32_t n) { return traits::plan_size(n); }
#endif
#ifdef FFT_TABLE_MAX_N_FLOAT
#ifndef FFT_PLAN_SIZE_FLOAT
	/* With --fft-table-only, plans n point transforms from the generated
	 * table, the same as from_table(). Throws std::invalid_argument if n is
	 * not a power of two from 2 to table_max_n */
	explicit fft(uint32_t n) {
		fft_check_status(traits::plan_init_table(&plan, n));
	}
#endif
	/* Largest n supported by the generated twiddle table */
	static constexpr uint32_t table_max_n = traits::table_max_n;
	/* Plans n point transforms from the generated table, with no workspace.
	 * Throws std::invalid_argument if n is not a power of two from 2 to
	 * table_max_n */
	static fft from_table(uint32_t n) {
		fft f;
		fft_check_status(traits::plan_init_table(&f.plan, n));
		return f;
	}
#endif

	/* In place forward and inverse transforms of x, which must hold size()
	 * values. Throws std::invalid_argument if it doesn't */
	void forward(std::vector<complex_t> &x) const {
		check_length(x.size());
		traits::forward(&plan, x.data());
	}
	void inverse(std::vector<complex_t> &x) const {
		check_length(x.size());
		traits::inverse(&plan, x.data());
	}
	/* Out of place, with out resized to size() values. out may be the same
	 * vector as x */
	void forward(const std::vector<complex_t> &x,
				 std::vector<complex_t> &out) const {
		check_length(x.size());
		out = x;
		traits::forward(&plan, out.data());
	}
	void inverse(const std::vector<complex_t> &x,
				 std::vector<complex_t> &out) const {
		check_length(x.size());
		out = x;
		traits::inverse(&plan, out.data());
	}
	/* Pointer overloads, for transforming at an offset within a larger
	 * buffer. x must hold size() values */
	void forward(complex_t *x) const { traits::forward(&plan, x); }
	void inverse(complex_t *x) const { traits::inverse(&plan, x); }
};

/* FFT of n real samples and its inverse, n a power of two of at least 2. About
 * half the work of fft for the same n. The in place transforms use the packed
 * layout of rfft_*() (x[0] bin 0, x[1] bin n / 2, then the real and imaginary
 * parts of bins 1 to n / 2 - 1), and the out of place ones give the n / 2 + 1
 * bins unpacked, as numpy.fft.rfft and numpy.fft.irfft */
template <typename num_t>
class rfft
	: public fft_plan_base<num_t, typename fft_traits<num_t>::rplan_type> {
  private:
	using base = fft_plan_base<num_t, typename fft_traits<num_t>::rplan_type>;
	using traits = typename base::traits;
	using complex_t = typename base::complex_t;
	using base::check_length;
	using base::plan;

	rfft() = default;

  public:
	using complex_type = complex_t;

#ifdef FFT_PLAN_SIZE_FLOAT
	/* Plans transforms of n real samples with a workspace owned by the object,
	 * throws std::invalid_argument if n is not a power of two of at least 2 */
	explicit rfft(uint32_t n) { this->init_owned(n, traits::rplan_init); }
	/* Uses the caller's workspace of work_bytes, which must outlive the
	 * object and its copies. Throws std::invalid_argument on any error from
	 * rfft_plan_init_*() */
	rfft(uint32_t n, void *work, size_t work_bytes) {
		fft_check_status(traits::rplan_init(&plan, n, work, work_bytes));
	}
	/* Workspace bytes needed for an n sample plan, 0 if n is invalid */
	static size_t workspace_size(uint32_t n) { return traits::plan_size(n); }
#endif
#ifdef FFT_TABLE_MAX_N_FLOAT
#ifndef FFT_PLAN_SIZE_FLOAT
	/* With --fft-table-only, plans from the generated table, the same as
	 * from_table() */
	explicit rfft(uint32_t n) {
		fft_check_status(traits::rplan_init_table(&plan, n));
	}
#endif
	/* Largest n supported by the generated twiddle table */
	static constexpr uint32_t table_max_n = traits::table_max_n;
	/* Plans transforms of n real samples from the generated table, with no
	 * workspace. Throws std::invalid_argument if n is not a power of two from
	 * 2 to table_max_n */
	static rfft from_table(uint32_t n) {
		rfft f;
		fft_check_status(traits::rplan_init_table(&f.plan, n));
		return f;
	}
#endif

	/* In place, in the packed layout. x must hold size() values, throws
	 * std::invalid_argument if it doesn't */
	void forward(std::vector<num_t> &x) const {
		check_length(x.size());
		traits::rforward(&plan, x.data());
	}
	void inverse(std::vector<num_t> &x) const {
		check_length(x.size());
		traits::rinverse(&plan, x.data());
	}
	/* Bins 0 to size() / 2 of the size() real samples in x, unpacked into
	 * bins, which is resized to size() / 2 + 1 */
	void forward(const std::vector<num_t> &x,
				 std::vector<complex_t> &bins) const {
		check_length(x.size());
		uint32_t half = plan.n / 2;
		/* complex_t is laid out as two num_t, so the packed bins 1 to
		 * half - 1 are already in place, and only bins 0 and half (both
		 * real) need separating */
		bins.resize(half + 1);
		forward(x.data(), bins.data());
	}
	/* Pointer overload, for transforming at an offset within a larger buffer.
	 * x must hold size() values and bins size() / 2 + 1, and they must not
	 * overlap */
	void forward(const num_t *x, complex_t *bins) const {
		uint32_t half = plan.n / 2;
		num_t *packed = reinterpret_cast<num_t *>(bins);
		std::memcpy(packed, x, plan.n * sizeof(num_t));
		traits::rforward(&plan, packed);
		bins[half] = {bins[0].imag, 0};
		bins[0].imag = 0;
	}
	/* The size() real samples from bins 0 to size() / 2, unpacked. x is
	 * resized to size(). The imaginary parts of bins 0 and size() / 2 are
	 * taken as 0. Throws std::invalid_argument if bins doesn't hold
	 * size() / 2 + 1 values */
	void inverse(const std::vector<complex_t> &bins,
				 std::vector<num_t> &x) const {
		uint32_t half = plan.n / 2;
		if (bins.size() != (std::size_t)half + 1) {
			throw std::invalid_argument(
				"rfft bins length isn't half the plan size plus 1");
		}
		x.resize(plan.n);
		inverse(bins.data(), x.data());
	}
	/* Pointer overload, bins must hold size() / 2 + 1 values and x size(),
	 * and they must not overlap */
	void inverse(const complex_t *bins, num_t *x) const {
		std::memcpy(x, bins, plan.n * sizeof(num_t));
		x[1] = bins[plan.n / 2].real;
		traits::rinverse(&plan, x);
	}
	/* Pointer overloads of the packed transforms, for transforming at an
	 * offset within a larger buffer. x must hold size() values */
	void forward(num_t *x) const { traits::rforward(&plan, x); }
	void inverse(num_t *x) const { traits::rinverse(&plan, x); }
};

#ifdef FFT_PLAN_SIZE_FLOAT
/* Direct O(N^2) DFT of any length, as a slow reference. out is resized to the
 * length of x, and must not be the same vector. Not generated with
 * --fft-table-only */
template <typename C, typename num_t = decltype(C::real)>
void dft(const std::vector<C> &x, std::vector<C> &out) {
	if (&x == &out) {
		throw std::invalid_argument("dft input and output are the same vector");
	}
	out.resize(x.size());
	fft_traits<num_t>::dft(x.data(), out.data(), (uint32_t)x.size());
}
/* Pointer overload, data_in and data_out must not overlap */
template <typename C, typename num_t = decltype(C::real)>
void dft(const C *data_in, C *data_out, uint32_t length) {
	fft_traits<num_t>::dft(data_in, data_out, length);
}
#endif

#endif /* DSP_TOOLBOX_CPP_WRAPPER_FFT_H */

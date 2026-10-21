#ifndef DSP_TOOLBOX_CPP_WRAPPER_COMPLEX_H
#define DSP_TOOLBOX_CPP_WRAPPER_COMPLEX_H

/* C++ wrapper for complex maths (complex_t, polar_t, the complex_* functions and
 * operators) */

#include "../inc/complex_double.h"
#include "../inc/complex_float.h"
#include "../inc/complex_int32.h"
#include "../inc/complex_int64.h"

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>

/* Map the numerical type (float/double/int32_t/int64_t) to struct and
 * function types for complex maths */
template <typename num_t> struct complex_traits;
/* Float Specialization */
template <> struct complex_traits<float> {
	using complex_type = complex_float_t;
	using polar_type = polar_float_t;
	/* Type of the phase */
	using math_type = float;

	static inline complex_type add(complex_type x, complex_type y) {
		return complex_add_float(x, y);
	}
	static inline complex_type subtract(complex_type x, complex_type y) {
		return complex_subtract_float(x, y);
	}
	static inline complex_type multiply(complex_type x, complex_type y) {
		return complex_multiply_float(x, y);
	}
	static inline complex_type divide(complex_type x, complex_type y) {
		return complex_divide_float(x, y);
	}
	static inline complex_type conjugate(complex_type x) {
		return complex_conjugate_float(x);
	}
	static inline float magnitude(complex_type x) {
		return complex_magnitude_float(x);
	}
	static inline math_type phase(complex_type x) {
		return complex_phase_float(x);
	}
	static inline polar_type to_polar(complex_type x) {
		return complex_to_polar_float(x);
	}
	static inline complex_type from_polar(polar_type p) {
		return complex_from_polar_float(p);
	}
	static inline void add_array(const complex_type *x, const complex_type *y, complex_type *out,
								 uint32_t len) {
		complex_add_array_float(x, y, out, len);
	}
	static inline void subtract_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len) {
		complex_subtract_array_float(x, y, out, len);
	}
	static inline void multiply_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len) {
		complex_multiply_array_float(x, y, out, len);
	}
	static inline void divide_array(const complex_type *x, const complex_type *y,
									complex_type *out, uint32_t len) {
		complex_divide_array_float(x, y, out, len);
	}
	static inline void conjugate_array(const complex_type *x, complex_type *out, uint32_t len) {
		complex_conjugate_array_float(x, out, len);
	}
	static inline void magnitude_array(const complex_type *x, float *out, uint32_t len) {
		complex_magnitude_array_float(x, out, len);
	}
	static inline void phase_array(const complex_type *x, math_type *out, uint32_t len) {
		complex_phase_array_float(x, out, len);
	}
	static inline void to_polar_array(const complex_type *x, polar_type *out, uint32_t len) {
		complex_to_polar_array_float(x, out, len);
	}
	static inline void from_polar_array(const polar_type *p, complex_type *out, uint32_t len) {
		complex_from_polar_array_float(p, out, len);
	}
};
/* Double Specialization */
template <> struct complex_traits<double> {
	using complex_type = complex_double_t;
	using polar_type = polar_double_t;
	/* Type of the phase */
	using math_type = double;

	static inline complex_type add(complex_type x, complex_type y) {
		return complex_add_double(x, y);
	}
	static inline complex_type subtract(complex_type x, complex_type y) {
		return complex_subtract_double(x, y);
	}
	static inline complex_type multiply(complex_type x, complex_type y) {
		return complex_multiply_double(x, y);
	}
	static inline complex_type divide(complex_type x, complex_type y) {
		return complex_divide_double(x, y);
	}
	static inline complex_type conjugate(complex_type x) {
		return complex_conjugate_double(x);
	}
	static inline double magnitude(complex_type x) {
		return complex_magnitude_double(x);
	}
	static inline math_type phase(complex_type x) {
		return complex_phase_double(x);
	}
	static inline polar_type to_polar(complex_type x) {
		return complex_to_polar_double(x);
	}
	static inline complex_type from_polar(polar_type p) {
		return complex_from_polar_double(p);
	}
	static inline void add_array(const complex_type *x, const complex_type *y, complex_type *out,
								 uint32_t len) {
		complex_add_array_double(x, y, out, len);
	}
	static inline void subtract_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len) {
		complex_subtract_array_double(x, y, out, len);
	}
	static inline void multiply_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len) {
		complex_multiply_array_double(x, y, out, len);
	}
	static inline void divide_array(const complex_type *x, const complex_type *y,
									complex_type *out, uint32_t len) {
		complex_divide_array_double(x, y, out, len);
	}
	static inline void conjugate_array(const complex_type *x, complex_type *out, uint32_t len) {
		complex_conjugate_array_double(x, out, len);
	}
	static inline void magnitude_array(const complex_type *x, double *out, uint32_t len) {
		complex_magnitude_array_double(x, out, len);
	}
	static inline void phase_array(const complex_type *x, math_type *out, uint32_t len) {
		complex_phase_array_double(x, out, len);
	}
	static inline void to_polar_array(const complex_type *x, polar_type *out, uint32_t len) {
		complex_to_polar_array_double(x, out, len);
	}
	static inline void from_polar_array(const polar_type *p, complex_type *out, uint32_t len) {
		complex_from_polar_array_double(p, out, len);
	}
};
/* Int32 Specialization */
template <> struct complex_traits<int32_t> {
	using complex_type = complex_int32_t;
	using polar_type = polar_int32_t;
	/* Floating point type of the phase, as radians can't be held in int32_t */
	using math_type = float;

	static inline complex_type add(complex_type x, complex_type y) {
		return complex_add_int32(x, y);
	}
	static inline complex_type subtract(complex_type x, complex_type y) {
		return complex_subtract_int32(x, y);
	}
	static inline complex_type multiply(complex_type x, complex_type y, uint32_t frac_bits) {
		return complex_multiply_int32(x, y, frac_bits);
	}
	static inline complex_type divide(complex_type x, complex_type y, uint32_t frac_bits) {
		return complex_divide_int32(x, y, frac_bits);
	}
	static inline complex_type conjugate(complex_type x) {
		return complex_conjugate_int32(x);
	}
	static inline int32_t magnitude(complex_type x) {
		return complex_magnitude_int32(x);
	}
	static inline math_type phase(complex_type x) {
		return complex_phase_int32(x);
	}
	static inline polar_type to_polar(complex_type x) {
		return complex_to_polar_int32(x);
	}
	static inline complex_type from_polar(polar_type p) {
		return complex_from_polar_int32(p);
	}
	static inline void add_array(const complex_type *x, const complex_type *y, complex_type *out,
								 uint32_t len) {
		complex_add_array_int32(x, y, out, len);
	}
	static inline void subtract_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len) {
		complex_subtract_array_int32(x, y, out, len);
	}
	static inline void multiply_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len, uint32_t frac_bits) {
		complex_multiply_array_int32(x, y, out, len, frac_bits);
	}
	static inline void divide_array(const complex_type *x, const complex_type *y,
									complex_type *out, uint32_t len, uint32_t frac_bits) {
		complex_divide_array_int32(x, y, out, len, frac_bits);
	}
	static inline void conjugate_array(const complex_type *x, complex_type *out, uint32_t len) {
		complex_conjugate_array_int32(x, out, len);
	}
	static inline void magnitude_array(const complex_type *x, int32_t *out, uint32_t len) {
		complex_magnitude_array_int32(x, out, len);
	}
	static inline void phase_array(const complex_type *x, math_type *out, uint32_t len) {
		complex_phase_array_int32(x, out, len);
	}
	static inline void to_polar_array(const complex_type *x, polar_type *out, uint32_t len) {
		complex_to_polar_array_int32(x, out, len);
	}
	static inline void from_polar_array(const polar_type *p, complex_type *out, uint32_t len) {
		complex_from_polar_array_int32(p, out, len);
	}
};
/* Int64 Specialization */
template <> struct complex_traits<int64_t> {
	using complex_type = complex_int64_t;
	using polar_type = polar_int64_t;
	/* Floating point type of the phase, as radians can't be held in int64_t */
	using math_type = double;

	static inline complex_type add(complex_type x, complex_type y) {
		return complex_add_int64(x, y);
	}
	static inline complex_type subtract(complex_type x, complex_type y) {
		return complex_subtract_int64(x, y);
	}
	static inline complex_type multiply(complex_type x, complex_type y, uint32_t frac_bits) {
		return complex_multiply_int64(x, y, frac_bits);
	}
	static inline complex_type divide(complex_type x, complex_type y, uint32_t frac_bits) {
		return complex_divide_int64(x, y, frac_bits);
	}
	static inline complex_type conjugate(complex_type x) {
		return complex_conjugate_int64(x);
	}
	static inline int64_t magnitude(complex_type x) {
		return complex_magnitude_int64(x);
	}
	static inline math_type phase(complex_type x) {
		return complex_phase_int64(x);
	}
	static inline polar_type to_polar(complex_type x) {
		return complex_to_polar_int64(x);
	}
	static inline complex_type from_polar(polar_type p) {
		return complex_from_polar_int64(p);
	}
	static inline void add_array(const complex_type *x, const complex_type *y, complex_type *out,
								 uint32_t len) {
		complex_add_array_int64(x, y, out, len);
	}
	static inline void subtract_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len) {
		complex_subtract_array_int64(x, y, out, len);
	}
	static inline void multiply_array(const complex_type *x, const complex_type *y,
									  complex_type *out, uint32_t len, uint32_t frac_bits) {
		complex_multiply_array_int64(x, y, out, len, frac_bits);
	}
	static inline void divide_array(const complex_type *x, const complex_type *y,
									complex_type *out, uint32_t len, uint32_t frac_bits) {
		complex_divide_array_int64(x, y, out, len, frac_bits);
	}
	static inline void conjugate_array(const complex_type *x, complex_type *out, uint32_t len) {
		complex_conjugate_array_int64(x, out, len);
	}
	static inline void magnitude_array(const complex_type *x, int64_t *out, uint32_t len) {
		complex_magnitude_array_int64(x, out, len);
	}
	static inline void phase_array(const complex_type *x, math_type *out, uint32_t len) {
		complex_phase_array_int64(x, out, len);
	}
	static inline void to_polar_array(const complex_type *x, polar_type *out, uint32_t len) {
		complex_to_polar_array_int64(x, out, len);
	}
	static inline void from_polar_array(const polar_type *p, complex_type *out, uint32_t len) {
		complex_from_polar_array_int64(p, out, len);
	}
};

/* The C complex and polar structs for num_t, e.g. complex_t<float> is
 * complex_float_t */
template <typename num_t>
using complex_t = typename complex_traits<num_t>::complex_type;
template <typename num_t>
using polar_t = typename complex_traits<num_t>::polar_type;

/* Map the C complex and polar structs back to their numerical type. These
 * are empty for any other type, so the functions below only accept the
 * complex structs and the numerical type is deduced from the arguments */
template <typename complex_type> struct complex_num_type {};
template <> struct complex_num_type<complex_float_t> { using type = float; };
template <> struct complex_num_type<complex_double_t> { using type = double; };
template <> struct complex_num_type<complex_int32_t> { using type = int32_t; };
template <> struct complex_num_type<complex_int64_t> { using type = int64_t; };
template <typename polar_type> struct polar_num_type {};
template <> struct polar_num_type<polar_float_t> { using type = float; };
template <> struct polar_num_type<polar_double_t> { using type = double; };
template <> struct polar_num_type<polar_int32_t> { using type = int32_t; };
template <> struct polar_num_type<polar_int64_t> { using type = int64_t; };

/* The traits of a complex struct, and return type R only if C is a complex
 * struct (of a floating point or integer type for the last two) */
template <typename C>
using complex_traits_of = complex_traits<typename complex_num_type<C>::type>;
template <typename C, typename R = C>
using if_complex = std::enable_if_t<
	std::is_same<C, typename complex_traits_of<C>::complex_type>::value, R>;
template <typename C, typename R = C>
using if_complex_float = std::enable_if_t<
	std::is_floating_point<typename complex_num_type<C>::type>::value, R>;
template <typename C, typename R = C>
using if_complex_fixed = std::enable_if_t<
	std::is_integral<typename complex_num_type<C>::type>::value, R>;

/* Single complex numbers. The integer types are fixed point, and multiply
 * and divide take the number of fractional bits of the format, see the C API
 * reference */
template <typename C> if_complex<C> complex_add(C x, C y) {
	return complex_traits_of<C>::add(x, y);
}
template <typename C> if_complex<C> complex_subtract(C x, C y) {
	return complex_traits_of<C>::subtract(x, y);
}
template <typename C> if_complex_float<C> complex_multiply(C x, C y) {
	return complex_traits_of<C>::multiply(x, y);
}
template <typename C>
if_complex_fixed<C> complex_multiply(C x, C y, uint32_t frac_bits) {
	return complex_traits_of<C>::multiply(x, y, frac_bits);
}
template <typename C> if_complex_float<C> complex_divide(C x, C y) {
	return complex_traits_of<C>::divide(x, y);
}
template <typename C>
if_complex_fixed<C> complex_divide(C x, C y, uint32_t frac_bits) {
	return complex_traits_of<C>::divide(x, y, frac_bits);
}
template <typename C> if_complex<C> complex_conjugate(C x) {
	return complex_traits_of<C>::conjugate(x);
}
template <typename C>
if_complex<C, typename complex_num_type<C>::type> complex_magnitude(C x) {
	return complex_traits_of<C>::magnitude(x);
}
/* The phase is in radians, float for int32_t and double for int64_t */
template <typename C>
if_complex<C, typename complex_traits_of<C>::math_type> complex_phase(C x) {
	return complex_traits_of<C>::phase(x);
}
template <typename C>
if_complex<C, typename complex_traits_of<C>::polar_type> complex_to_polar(C x) {
	return complex_traits_of<C>::to_polar(x);
}
template <typename P>
complex_t<typename polar_num_type<P>::type> complex_from_polar(P p) {
	return complex_traits<typename polar_num_type<P>::type>::from_polar(p);
}

/* Operators for the complex structs. * and / are only for float and double,
 * as the integer types need the fractional bits */
template <typename C> if_complex<C> operator+(C x, C y) {
	return complex_add(x, y);
}
template <typename C> if_complex<C> operator-(C x, C y) {
	return complex_subtract(x, y);
}
template <typename C> if_complex_float<C> operator*(C x, C y) {
	return complex_multiply(x, y);
}
template <typename C> if_complex_float<C> operator/(C x, C y) {
	return complex_divide(x, y);
}

/* Vector versions, applying the function to each element. out is resized to
 * the length of x, and may be the same vector as x or y. Throws
 * std::invalid_argument if x and y are different lengths */
template <typename C>
void complex_check_lengths(const std::vector<C> &x, const std::vector<C> &y) {
	if (x.size() != y.size()) {
		throw std::invalid_argument("complex vectors are different lengths");
	}
}
template <typename C>
if_complex<C, void> complex_add(const std::vector<C> &x,
								const std::vector<C> &y, std::vector<C> &out) {
	complex_check_lengths(x, y);
	out.resize(x.size());
	complex_traits_of<C>::add_array(x.data(), y.data(), out.data(),
									(uint32_t)x.size());
}
template <typename C>
if_complex<C, void> complex_subtract(const std::vector<C> &x,
									 const std::vector<C> &y,
									 std::vector<C> &out) {
	complex_check_lengths(x, y);
	out.resize(x.size());
	complex_traits_of<C>::subtract_array(x.data(), y.data(), out.data(),
										 (uint32_t)x.size());
}
template <typename C>
if_complex_float<C, void> complex_multiply(const std::vector<C> &x,
										   const std::vector<C> &y,
										   std::vector<C> &out) {
	complex_check_lengths(x, y);
	out.resize(x.size());
	complex_traits_of<C>::multiply_array(x.data(), y.data(), out.data(),
										 (uint32_t)x.size());
}
template <typename C>
if_complex_fixed<C, void> complex_multiply(const std::vector<C> &x,
										   const std::vector<C> &y,
										   std::vector<C> &out,
										   uint32_t frac_bits) {
	complex_check_lengths(x, y);
	out.resize(x.size());
	complex_traits_of<C>::multiply_array(x.data(), y.data(), out.data(),
										 (uint32_t)x.size(), frac_bits);
}
template <typename C>
if_complex_float<C, void> complex_divide(const std::vector<C> &x,
										 const std::vector<C> &y,
										 std::vector<C> &out) {
	complex_check_lengths(x, y);
	out.resize(x.size());
	complex_traits_of<C>::divide_array(x.data(), y.data(), out.data(),
									   (uint32_t)x.size());
}
template <typename C>
if_complex_fixed<C, void> complex_divide(const std::vector<C> &x,
										 const std::vector<C> &y,
										 std::vector<C> &out,
										 uint32_t frac_bits) {
	complex_check_lengths(x, y);
	out.resize(x.size());
	complex_traits_of<C>::divide_array(x.data(), y.data(), out.data(),
									   (uint32_t)x.size(), frac_bits);
}
template <typename C>
if_complex<C, void> complex_conjugate(const std::vector<C> &x,
									  std::vector<C> &out) {
	out.resize(x.size());
	complex_traits_of<C>::conjugate_array(x.data(), out.data(),
										  (uint32_t)x.size());
}
template <typename C>
if_complex<C, void>
complex_magnitude(const std::vector<C> &x,
				  std::vector<typename complex_num_type<C>::type> &out) {
	out.resize(x.size());
	complex_traits_of<C>::magnitude_array(x.data(), out.data(),
										  (uint32_t)x.size());
}
template <typename C>
if_complex<C, void>
complex_phase(const std::vector<C> &x,
			  std::vector<typename complex_traits_of<C>::math_type> &out) {
	out.resize(x.size());
	complex_traits_of<C>::phase_array(x.data(), out.data(), (uint32_t)x.size());
}
template <typename C>
if_complex<C, void>
complex_to_polar(const std::vector<C> &x,
				 std::vector<typename complex_traits_of<C>::polar_type> &out) {
	out.resize(x.size());
	complex_traits_of<C>::to_polar_array(x.data(), out.data(),
										 (uint32_t)x.size());
}
template <typename P>
void complex_from_polar(const std::vector<P> &p,
						std::vector<complex_t<typename polar_num_type<P>::type>> &out) {
	out.resize(p.size());
	complex_traits<typename polar_num_type<P>::type>::from_polar_array(
		p.data(), out.data(), (uint32_t)p.size());
}

/* Pointer overloads of the vector versions, for working at an offset within
 * a larger buffer without copying. out may be the same buffer as x or y */
template <typename C>
if_complex<C, void> complex_add(const C *x, const C *y, C *out,
								uint32_t length) {
	complex_traits_of<C>::add_array(x, y, out, length);
}
template <typename C>
if_complex<C, void> complex_subtract(const C *x, const C *y, C *out,
									 uint32_t length) {
	complex_traits_of<C>::subtract_array(x, y, out, length);
}
template <typename C>
if_complex_float<C, void> complex_multiply(const C *x, const C *y, C *out,
										   uint32_t length) {
	complex_traits_of<C>::multiply_array(x, y, out, length);
}
template <typename C>
if_complex_fixed<C, void> complex_multiply(const C *x, const C *y, C *out,
										   uint32_t length,
										   uint32_t frac_bits) {
	complex_traits_of<C>::multiply_array(x, y, out, length, frac_bits);
}
template <typename C>
if_complex_float<C, void> complex_divide(const C *x, const C *y, C *out,
										 uint32_t length) {
	complex_traits_of<C>::divide_array(x, y, out, length);
}
template <typename C>
if_complex_fixed<C, void> complex_divide(const C *x, const C *y, C *out,
										 uint32_t length, uint32_t frac_bits) {
	complex_traits_of<C>::divide_array(x, y, out, length, frac_bits);
}
template <typename C>
if_complex<C, void> complex_conjugate(const C *x, C *out, uint32_t length) {
	complex_traits_of<C>::conjugate_array(x, out, length);
}
template <typename C>
if_complex<C, void> complex_magnitude(const C *x,
									  typename complex_num_type<C>::type *out,
									  uint32_t length) {
	complex_traits_of<C>::magnitude_array(x, out, length);
}
template <typename C>
if_complex<C, void>
complex_phase(const C *x, typename complex_traits_of<C>::math_type *out,
			  uint32_t length) {
	complex_traits_of<C>::phase_array(x, out, length);
}
template <typename C>
if_complex<C, void>
complex_to_polar(const C *x, typename complex_traits_of<C>::polar_type *out,
				 uint32_t length) {
	complex_traits_of<C>::to_polar_array(x, out, length);
}
template <typename P>
void complex_from_polar(const P *p,
						complex_t<typename polar_num_type<P>::type> *out,
						uint32_t length) {
	complex_traits<typename polar_num_type<P>::type>::from_polar_array(
		p, out, length);
}

#endif /* DSP_TOOLBOX_CPP_WRAPPER_COMPLEX_H */

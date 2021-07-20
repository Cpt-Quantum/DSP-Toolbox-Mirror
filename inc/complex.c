#include <stdint.h>
#include <math.h>

#include "complex.h"

complex_t complex_multiply (complex_t x, complex_t y) 
{
	complex_t ret;

	ret.real = (x.real * y.real) - (x.imag * y.imag);
	ret.imag = (x.real * y.imag) + (y.real * x.imag);

	return ret;
}

complex_t complex_add (complex_t x, complex_t y)
{
	complex_t ret;

	ret.real = x.real + y.real;
	ret.imag = x.imag + y.imag;

	return ret;
}

complex_t complex_conjugate (complex_t x)
{
	complex_t ret;

	ret.real = x.real;
	ret.imag = - x.imag;

	return ret;
}

float complex_magnitude (complex_t x)
{
	return sqrt( (x.real * x.real) + (x.imag * x.imag) );
}

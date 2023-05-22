#ifndef COMPLEX_H
#define COMPLEX_H

#include <stdint.h>
#include <math.h>

/* This defines the type for a single complex number */
typedef struct {
	float real;
	float imag;
} complex_t;

complex_t complex_multiply (complex_t x, complex_t y);
complex_t complex_add (complex_t x, complex_t y);
complex_t complex_conjugate (complex_t x);
float complex_magnitude (complex_t x);

#endif //COMPLEX_H

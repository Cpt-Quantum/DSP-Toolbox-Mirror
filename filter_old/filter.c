#include <math.h>
#include <stdlib.h>

#include "filter.h"

/* Define filter objects to point at */
float filt_a[] = {1.0, -3.187435145652140, 4.437529217427072, -3.025620552651779,  0.901108152734037};
float filt_b[] = {0.949266292642437, -3.106527849151959, 4.440104784876235, -3.106527849151959, 0.949266292642437};
#define FILT_TAPS sizeof(filt_a)/sizeof(float)
float filt_in_buff[FILT_TAPS] = {0};
float filt_out_buff[FILT_TAPS] = {0};

struct filter filt = {
	.a = filt_a,
	.b = filt_b,
	.in_buff = filt_in_buff,
	.out_buff = filt_out_buff,
	.taps = FILT_TAPS,
	.cycle = 0
};


void filter_sig(float *x, float *y, struct filter *filt, uint32_t data_length)
{
	/* Ensure the output signal is initialised */
	for (uint32_t i = 0; i < data_length; i++)
	{
		y[i] = 0;
	}

	uint16_t N = filt->taps;

	for (uint32_t i = 0; i < data_length; i++)
	{
		uint16_t c = filt->cycle;
		filt->in_buff[c % N] = x[i];

		/* Handle the b coefficients */
		for (uint32_t j = 0; j < N; j++)
		{
			y[i] = y[i] + filt->b[j] * filt->in_buff[(c - i + N) % N];
		}
		/* Handle the a coefficients */
		for (uint32_t j = 1; j < N; j++)
		{
			y[i] = y[i] - filt->a[j] * filt->out_buff[(c - i + N) % N];
		}
		filt->out_buff[c % N] = y[i];
		filt->cycle = (c + 1) % N;
	}
}

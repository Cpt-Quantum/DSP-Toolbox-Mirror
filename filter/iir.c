#include <math.h>
#include <stdlib.h>

#include "iir.h"

/* Set the coefficients for each of the second order sections */
/* TODO: Replace these coefficient assignments with real ones */
float sos_a[] = {1, 1, 1};
float sos_b[] = {1, 1, 1};

struct biquad_section_t biquads[] = {
	[0] = {
		.a = {1, -1.8141, 0.82381},
		.b = {1, 2.0024, 1.0024},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[1] = {
		.a = {1, -1.8512, 0.8634},
		.b = {1, 1.9976, 0.99762},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[2] = {
		.a = {1, -1.9337, 0.94801},
		.b = {1, -1.9976, 0.99759},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[3] = {
		.a = {1, -1.9453, 0.95086},
		.b = {1, -2.0024, 1.0024},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[4] = {
		.a = {1, -2.053, 1.0556},
		.b = {1, 2, 1},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[5] = {
		.a = {1, -1.9673, 0.96407},
		.b = {1, -2, 1},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
};

struct sos_filter_t sos_filt = {
	.biquad_section = biquads,
	.num_sections = 5,
	.gain = 2.3050e-08,
};

void iir_filter(float *data_in, float *data_out, struct iir_t *filter, uint32_t length)
{
	uint8_t N = filter->taps;
	uint32_t c, i, j;

	for (j = 0; j < length; j++)
	{
		data_out[j] = 0;
	}

	for (c = 0; c < length; c++)
	{
		filter->in_buff[(c + filter->cycle) % N] = data_in[c];
		for (i = 0; i < N; i++)
		{
			data_out[c] = data_out[c] + (filter->iir_coeff->b[i] * filter->in_buff[(c + filter->cycle - i + N) % N]);
		}
		for (i = 1; i < N; i++)
		{
			data_out[c] = data_out[c] - (filter->iir_coeff->a[i] * filter->out_buff[(c + filter->cycle - i + N) % N]);
		}

		data_out[c] = data_out[c] / filter->iir_coeff->a[0];
		filter->out_buff[(c + filter->cycle) % N] = data_out[c];
	}
	filter->cycle = c + filter->cycle;
}

/* All biquad sections will have 3 coefficients on each denominator: */
/* a0, a1, a2, b0, b1, b2 */
#define BIQUAD_FILT_TAPS 3
void biquad_section_filter(float *data_in, float *data_out,
						   struct biquad_section_t *section, uint32_t length)
{
	uint32_t c, i, j;

	//	for (j = 0; j < length; j++)
	//	{
	//		data_out[j] = 0;
	//	}

	for (c = 0; c < length; c++)
	{
		section->in_buff[(c + section->cycle) % BIQUAD_FILT_TAPS] = data_in[c];
		for (i = 0; i < BIQUAD_FILT_TAPS; i++)
		{
			data_out[c] = data_out[c] + (section->b[i] * section->in_buff[(c + section->cycle - i + BIQUAD_FILT_TAPS) % BIQUAD_FILT_TAPS]);
		}
		for (i = 1; i < BIQUAD_FILT_TAPS; i++)
		{
			data_out[c] = data_out[c] - (section->a[i] * section->out_buff[(c + section->cycle - i + BIQUAD_FILT_TAPS) % BIQUAD_FILT_TAPS]);
		}

		data_out[c] = data_out[c] / section->a[0];
		section->out_buff[(c + section->cycle) % BIQUAD_FILT_TAPS] = data_out[c];
	}
	section->cycle = c + section->cycle;
}

void sos_filter(float *data_in, float *data_out, struct sos_filter_t *filter, uint32_t length)
{
	/* Zero out the data_out buffer */
	for (uint32_t i = 0; i < length; i++)
	{
		data_out[i] = 0;
	}

	float data_in_buff[32];
	for (uint32_t i = 0; i < length; i++)
	{
		data_in_buff[i] = data_in[i];
	}
	for (uint32_t i = 0; i < filter->num_sections; i++)
	{
		for (uint32_t j = 0; j < length; j++)
		{
			data_in_buff[j] = filter->gain * data_out[j];
		}
		biquad_section_filter(data_in_buff, data_out, &filter->biquad_section[i], length);
	}
}

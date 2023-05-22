#include <math.h>
#include <stdlib.h>

#include "iir.h"

/* Set the coefficients for each of the second order sections */
struct biquad_section_t biquads[] = {
	[0] = {
		.a = {1, -1.6629829, 0.69242287},
		.b = {1, 2.0524712, 1.0532802},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[1] = {
		.a = {1, -1.7017719, 0.73286957},
		.b = {1, 2.0211606, 1.0219609},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[2] = {
		.a = {1, -1.7792711, 0.81312108},
		.b = {1, 1.9780388, 0.97882664},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[3] = {
		.a = {1, -1.8931551, 0.93011516},
		.b = {1, 1.9483294, 0.94910896},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[4] = {
		.a = {1, -1.9899222, 0.98994863},
		.b = {1, -2.0000687, 1.0000685},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[5] = {
		.a = {1, -1.9917027, 0.99172837},
		.b = {1, -2.0000279, 1.0000279},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[6] = {
		.a = {1, -1.9946512, 0.99467593},
		.b = {1, -1.9999322, 0.99993217},

		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
	[7] = {
		.a = {1, -1.9981492, 0.99817336},
		.b = {1, -1.9999714, 0.99997139},
		.in_buff = {0},
		.out_buff = {0},
		.cycle = 0,
	},
};

struct sos_filter_t sos_filt = {
	.biquad_section = biquads,
	.num_sections = 5,
	.gain = 4.4173465e-09,
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
	filter->cycle = (c + filter->cycle) % N;
}

/* All biquad sections will have 3 coefficients on each denominator: */
/* a0, a1, a2, b0, b1, b2 */
#define BIQUAD_FILT_TAPS 3
void biquad_section_filter(float *data_in, float *data_out,
						   struct biquad_section_t *section, uint32_t length)
{
	uint32_t c, i, j;

	for (j = 0; j < length; j++)
	{
		data_out[j] = 0;
	}

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
	section->cycle = (c + section->cycle) % BIQUAD_FILT_TAPS;
}

void sos_filter(float *data_in, float *data_out, struct sos_filter_t *filter, uint32_t length)
{

	float data_in_buff[32] = {0};
	for (uint32_t i = 0; i < length; i++)
	{
		data_in_buff[i] = data_in[i];
	}
	for (uint32_t i = 0; i < filter->num_sections; i++)
	{
		biquad_section_filter(data_in_buff, data_out, &filter->biquad_section[i], length);
		for (uint32_t j = 0; j < length; j++)
		{
			data_in_buff[j] = data_out[j];
		}
	}

	for (uint32_t j = 0; j < length; j++)
	{
		data_out[j] = filter->gain * data_out[j];
	}
}

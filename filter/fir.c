#include <math.h>
#include <stdlib.h>

#include "fir.h"

void fir_filter(float *data_in, float *data_out, struct fir_t *filter, uint32_t length)
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
			data_out[c] = data_out[c] + (filter->b[i] * filter->in_buff[(c + filter->cycle - i + N) % N]);
		}
	}
	filter->cycle = (c + filter->cycle) % N;
}

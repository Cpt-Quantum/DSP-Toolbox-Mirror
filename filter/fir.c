#include <stdint.h>

#include "fir.h"

void fir_filter(float *data_in, float *data_out, struct fir_t *filter,
				uint32_t length) {
	uint8_t N = filter->taps;
	/* Create index variables:
	 * c: Filter cycle index
	 * i: Input/Output Data index
	 */

	uint32_t c, i;

	for (i = 0; i < length; i++) {
		data_out[i] = 0;
	}

	for (c = 0; c < length; c++) {
		filter->in_buff[(c + filter->cycle) % N] = data_in[c];
		for (i = 0; i < N; i++) {
			data_out[c] = data_out[c] +
						  (filter->b[i] *
						   filter->in_buff[(c + filter->cycle - i + N) % N]);
		}
	}
	filter->cycle = (c + filter->cycle) % N;
}

void fir_decimate(float *data_in, float *data_out, struct fir_t *filter,
				  uint32_t length, uint8_t decimation_rate) {
	uint8_t N = filter->taps;
	/* Create index variables:
	 * c: Filter cycle index
	 * i: Input Data index
	 * j: Decimated data index
	 */
	uint32_t c, i, j;

	for (i = 0; i < length; i++) {
		data_out[i] = 0;
	}

	j = 0;
	for (c = 0; c < length; c++) {
		filter->in_buff[(c + filter->cycle) % N] = data_in[c];
		/* Only calculate the filter against the output values to avoid
		 * unnecessary computation */
		if (c % decimation_rate) {
			for (i = 0; i < N; i++) {
				data_out[j] =
					data_out[j] +
					(filter->b[i] *
					 filter->in_buff[(c + filter->cycle - i + N) % N]);
			}
			j++;
		}
	}
	filter->cycle = (c + filter->cycle) % N;
}

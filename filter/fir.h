#ifndef IIR_H
#define IIR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

struct fir_t
{
	float *b;
	float *in_buff;
	uint16_t taps;
	uint16_t cycle;
};

void fir_filter(float *data_in, float *data_out, struct fir_t *filter, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* IIR_H */

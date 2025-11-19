#ifndef FILTER_H
#define FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

/* NB: May want to change this struct to instead point to a struct containing */
/* a, b , in_buff and out_buff all in one structure. */
struct filter {
	float *a, *b;
	float *in_buff, *out_buff;
	uint16_t taps;
	uint16_t cycle;
};

extern struct filter filt;

void filter_sig (float *x, float *y, struct filter *filt, uint32_t data_length);

#ifdef __cplusplus
}
#endif

#endif	/* FILTER_H */

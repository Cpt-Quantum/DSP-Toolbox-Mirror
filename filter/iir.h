#ifndef IIR_H
#define IIR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct iir_coeff_t {
	float *a;
	float *b;
};

/* NB: May want to change this struct to instead point to a struct containing */
/* a, b , in_buff and out_buff all in one structure. */
struct iir_t {
	struct iir_coeff_t *iir_coeff;
	float *in_buff, *out_buff;
	uint16_t taps;
	uint16_t cycle;
};

/* Definitions for IIR filters in biquad/second order sections format */
struct biquad_section_t {
	float a[3], b[3];
	float in_buff[3], out_buff[3];
	uint16_t cycle;
};
struct sos_filter_t {
	struct biquad_section_t *biquad_section;
	uint16_t num_sections;
	float gain;
};

void iir_filter(float *data_in, float *data_out, struct iir_t *filter,
				uint32_t length);
void sos_filter(float *data_in, float *data_out,
				struct sos_filter_t *sos_filter, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* IIR_H */

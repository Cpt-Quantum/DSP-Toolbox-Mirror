#ifndef MAIN_INC_H
#define MAIN_INC_H

#include "fir_float.h"
#include "fir_int32.h"
#include "iir_float.h"
#include "iir_int32.h"

/* Make the necessary symbols visible publically */
extern float f_s;
extern struct sos_filter_float_t sos_filt;
extern struct iir_float_t iir_filt;
extern struct fir_float_t fir_filt;
extern struct fir_float_t fir_decimation_filt;
/* The same filters with Q format integer coefficients */
extern struct sos_filter_int32_t sos_filt_int32;
extern struct iir_int32_t iir_filt_int32;
extern struct fir_int32_t fir_filt_int32;
extern struct fir_int32_t fir_decimation_filt_int32;

void filter_init(const char *filename);
void filter_deinit();

#endif /* MAIN_INC_H */

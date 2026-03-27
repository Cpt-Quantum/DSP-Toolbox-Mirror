#ifndef MAIN_INC_H
#define MAIN_INC_H

#include "fir_float.h"
#include "iir_float.h"

/* Make the necessary symbols visible publically */
extern float f_s;
extern struct sos_filter_float_t sos_filt;
extern struct iir_float_t iir_filt;
extern struct fir_float_t fir_filt;
extern struct fir_float_t fir_decimation_filt;

void filter_init(const char *filename);
void filter_deinit();

#endif /* MAIN_INC_H */

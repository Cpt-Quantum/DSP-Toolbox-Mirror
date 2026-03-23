#ifndef MAIN_INC_H
#define MAIN_INC_H

#include "fir.h"
#include "iir.h"

/* Define the frequency at which the waveform is sampled */
#define SAMPLING_FREQUENCY 16000

/* Make the necessary symbols visible publically */
extern struct sos_filter_t sos_filt;
extern struct iir_t iir_filt;
extern struct fir_t fir_filt;

#endif /* MAIN_INC_H */

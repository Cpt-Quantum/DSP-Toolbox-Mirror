#ifndef FFT_H
#define FFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>

#include "../inc/complex_float.h"

void ft(complex_float_t data_in[], complex_float_t data_out[], int data_size);

//Radix-2 FFT
void fft(complex_float_t data_in[], complex_float_t data_out[], int data_size);

#ifdef __cplusplus
}
#endif

#endif //FFT_H

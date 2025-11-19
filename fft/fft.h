#ifndef FFT_H
#define FFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdint.h>

#include "../inc/complex.h"

void ft(complex_t data_in[], complex_t data_out[], int data_size);

//Radix-2 FFT
void fft(complex_t data_in[], complex_t data_out[], int data_size);

#ifdef __cplusplus
}
#endif

#endif //FFT_H

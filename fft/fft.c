#include <math.h>
#include <stdint.h>

#include "../inc/constants.h"
#include "../inc/complex.h"
#include "fft.h"

void ft(complex_t data_in[], complex_t data_out[], int data_size)
{
	for (int i = 0; i < data_size; i++)
	{
		for (int j = 0; j < data_size; j++)
		{
			float exponent = (pi2 / data_size) * i * j;
			/* Calculate the cos (c) and sin (s) parts ahead of use */
			float c = cos(exponent);
			float s = sin(exponent);

			data_out[i].real = data_out[i].real + (data_in[j].real * c + data_in[j].imag * s);
			data_out[i].imag = data_out[i].imag + (data_in[j].imag * c - data_in[j].real * s);
		}
	}

}

//Radix-2 FFT
//void fft(complex_t data_in[], complex_t data_out[], int data_size)
//{
//	complex_t E[data_size / 2];
//	complex_t O[data_size / 2];
//
//	int n;
//	int m;
//	int k;
//
//	/* Temporary stores for the exponent of e^(), cos and sine */
//	/* This saves on a few multiplications per cycle */
//	float exponent;
//	float c;
//	float s;
//
//
//	//float W = (pi2 * 2) / data_size;
//
//	for (k = 0; k < data_size / 2; k++)
//	{
//		/* Compute the even numbered DFT */
//		for (n = 0; n < data_size / 2; n++)
//		{
//			m = 2 * n;
//			exponent = (pi2 * 2 * m * k) / data_size;
//			c = cos(exponent);
//			s = sin(exponent);
//
//			E[k].real = E[k].real + (data_in[m].real * c + data_in[m].imag * s);
//			E[k].imag = E[k].imag + (data_in[m].imag * c - data_in[m].real * s);
//		}
//
//		/* Compute the odd numbered DFT */
//		for (n = 0; n < ((data_size / 2) - 1); n++)
//		{
//			m = 2 * n + 1;
//			exponent = (pi2 * 2 * m * k) / data_size;
//			c = cos(exponent);
//			s = sin(exponent);
//
//			O[k].real = O[k].real + (data_in[m].real * c + data_in[m].imag * s);
//			O[k].imag = O[k].imag + (data_in[m].imag * c - data_in[m].real * s);
//		}
//	}
//
//	/* Apply the coefficients E[] and O[] to each k value */
//	for (k = 0; k < data_size / 2; k++)
//	{
//		exponent = (pi2 * k) / data_size;
//		c = cos(exponent);
//		s = sin(exponent);
//
//		data_out[k].real = E[k].real + (c * O[k].real + s * O[k].imag);
//		data_out[k].imag = E[k].imag + (c * O[k].imag - s * O[k].real);
//		data_out[k + (data_size / 2)].real = E[k].real - (c * O[k].real + s * O[k].imag);
//		data_out[k + (data_size / 2)].imag = E[k].imag + (c * O[k].imag - s * O[k].real);
//	}
//}
void fft(complex_t data_in[], complex_t data_out[], int data_size)
{

}
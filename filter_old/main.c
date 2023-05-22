#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "filter.h"
#include "../inc/constants.h"
#include "../inc/complex.h"
#include "../inc/waveform_gen.h"

/* Define the parameters of the input waveform */
#define AMPL_1 10
#define FREQ_1 10
#define PHASE_1 0

#define AMPL_2 10
#define FREQ_2 50
#define PHASE_2 0

/* Define the frequency at which the waveform is sampled */
#define SAMPLING_FREQUENCY 512

/* Define the length of the data to work on */
#define DATA_SIZE 8192

const char in_filename[] = "input_data.csv";
const char out_filename[] = "output_data.csv";

int main()
{
	float t[DATA_SIZE];
	float x[DATA_SIZE];
	float y[DATA_SIZE];

	/* Initialise the arrays to zero */
	for(uint32_t i = 0; i < DATA_SIZE; i++)
	{
		x[i] = 0;
		y[i] = 0;
	}

	/* Generate time and data */
	gen_time(t, DATA_SIZE, SAMPLING_FREQUENCY);
	add_cos(t, x, DATA_SIZE, FREQ_1, AMPL_1, PHASE_1);
	add_cos(t, x, DATA_SIZE, FREQ_2, AMPL_2, PHASE_2);

	/* Perform the filter */
	filter_sig(x, y, &filt, DATA_SIZE);
	
	/* Print out the data to file */
	FILE *in_file = fopen(in_filename, "w");
	
	fprintf(in_file, "Time,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++)
	{
		fprintf(in_file, "%f,%f\n", t[i], x[i]);
	}

	fclose(in_file);

	FILE *out_file = fopen(out_filename, "w");
	fprintf(out_file, "Time,Amplitude\n");
	for(int i = 0; i < DATA_SIZE; i++)
	{
		fprintf(out_file, "%f,%f\n", t[i], y[i]);
	}
	
	fclose(out_file);

	return 0;
}

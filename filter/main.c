#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "../inc/complex_float.h"
#include "../inc/constants_float.h"
#include "../inc/waveform_gen_float.h"
#include "iir.h"

/* Define the frequency at which the waveform is sampled */
#define SAMPLING_FREQUENCY 64

/* Define the length of the data to work on */
#define DATA_CHUNK_SIZE 16
#define DATA_CHUNK_COUNT (SAMPLING_FREQUENCY * 100)
#define DATA_SIZE (DATA_CHUNK_SIZE * DATA_CHUNK_COUNT)

/* Define the parameters of the input waveform */
static wave_properties_float_t wave_1[] = {
	[0] = {.amplitude = 10,
		   .offset = 0,
		   .frequency = 0.005,
		   .phase = 0,
		   .function_type = FUNCTION_SINE_FLOAT},
	[1] = {.amplitude = 10,
		   .offset = 0,
		   .frequency = 0.8,
		   .phase = 0,
		   .function_type = FUNCTION_SINE_FLOAT},
	[2] = {.amplitude = 10,
		   .offset = 0,
		   .frequency = 5,
		   .phase = 0,
		   .function_type = FUNCTION_SINE_FLOAT},
	//[3] = {.amplitude = 50, .offset = 0, .frequency = 100, .phase = 0,
	//.function_type = FUNCTION_COS_FLOAT},
};
static const wave_settings_float_t wave_1_settings = {
	.fs = SAMPLING_FREQUENCY,
	.data_length = DATA_SIZE,
	.dc_offset = 0,
	.superposition = wave_1,
	.num_superpositions = sizeof(wave_1) / sizeof(wave_1[0]),
};

/* Define the filenames for the input data file and the output data file */
const char in_filename[] = "input_data.csv";
const char out_filename[] = "output_data.csv";

/* Define IIR filter parameters */
float filt_a[] = {1.0, -3.187435145652140, 4.437529217427072,
				  -3.025620552651779, 0.901108152734037};
float filt_b[] = {0.949266292642437, -3.106527849151959, 4.440104784876235,
				  -3.106527849151959, 0.949266292642437};
#define FILT_TAPS (sizeof(filt_a) / sizeof(filt_a[0]))
float filt_in_buff[FILT_TAPS] = {0};
float filt_out_buff[FILT_TAPS] = {0};
/* Set the coefficients for each of the second order sections */
struct biquad_section_t biquads[] = {
	[0] =
		{
			.a = {1, -1.6629829, 0.69242287},
			.b = {1, 2.0524712, 1.0532802},
			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
	[1] =
		{
			.a = {1, -1.7017719, 0.73286957},
			.b = {1, 2.0211606, 1.0219609},
			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
	[2] =
		{
			.a = {1, -1.7792711, 0.81312108},
			.b = {1, 1.9780388, 0.97882664},
			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
	[3] =
		{
			.a = {1, -1.8931551, 0.93011516},
			.b = {1, 1.9483294, 0.94910896},
			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
	[4] =
		{
			.a = {1, -1.9899222, 0.98994863},
			.b = {1, -2.0000687, 1.0000685},
			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
	[5] =
		{
			.a = {1, -1.9917027, 0.99172837},
			.b = {1, -2.0000279, 1.0000279},
			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
	[6] =
		{
			.a = {1, -1.9946512, 0.99467593},
			.b = {1, -1.9999322, 0.99993217},

			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
	[7] =
		{
			.a = {1, -1.9981492, 0.99817336},
			.b = {1, -1.9999714, 0.99997139},
			.in_buff = {0},
			.out_buff = {0},
			.cycle = 0,
		},
};

struct sos_filter_t sos_filt = {
	.biquad_section = biquads,
	.num_sections = 5,
	.gain = 4.4173465e-09,
};

struct iir_coeff_t iir_coeffs = {.a = filt_a, .b = filt_b};

struct iir_t filt = {.iir_coeff = &iir_coeffs,
					 .in_buff = filt_in_buff,
					 .out_buff = filt_out_buff,
					 .taps = FILT_TAPS,
					 .cycle = 0};

int main() {
	float t[DATA_SIZE];
	float x[DATA_SIZE];
	float y[DATA_SIZE];

	/* Initialise the arrays to zero */
	for (uint32_t i = 0; i < DATA_SIZE; i++) {
		x[i] = 0;
		y[i] = 0;
	}

	/* Generate time and data */
	gen_time_float(t, DATA_SIZE, SAMPLING_FREQUENCY);
	init_waveform_float(wave_1_settings, t, x);

	/* Perform the filter */
	/* The data is sent through in chunks to simulate live data */
	for (uint32_t i = 0; i < DATA_CHUNK_COUNT; i++) {
		uint32_t start_index = i * DATA_CHUNK_SIZE;
		// iir_filter(&x[start_index], &y[start_index], &filt, DATA_CHUNK_SIZE);
		sos_filter(&x[start_index], &y[start_index], &sos_filt,
				   DATA_CHUNK_SIZE);
	}
	/* Print out the data to file */
	FILE *in_file = fopen(in_filename, "w");

	fprintf(in_file, "Time,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++) {
		fprintf(in_file, "%f,%f\n", t[i], x[i]);
	}

	fclose(in_file);

	FILE *out_file = fopen(out_filename, "w");
	fprintf(out_file, "Time,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++) {
		fprintf(out_file, "%f,%f\n", t[i], y[i]);
	}

	fclose(out_file);

	return 0;
}

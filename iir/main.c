/* NOTE: Currently outputs NaN values in the signal_out array */

#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define SAMPLING_FREQUENCY 512

#define SINE_FREQ_1 2
#define SINE_FREQ_2	50
#define SINE_AMPL_1 10
#define SINE_AMPL_2 100

#define DATA_SIZE 1000

static const double pi = 3.141592654;
static const double pi2 = 6.283185307;
const char in_filename[] = "input_data.csv";
const char out_filename[] = "output_data.csv";

int main()
{

	double t[DATA_SIZE];
	double signal_in[DATA_SIZE];
	double signal_out[DATA_SIZE];

	/* Define the IIR filter coefficients and data structures */
	static const double a[] = { 1.0, -3.187435145652140, 4.437529217427072, -3.025620552651779,  0.901108152734037 };
	static const double b[] = { 0.949266292642437, -3.106527849151959, 4.440104784876235, -3.106527849151959, 0.949266292642437 };
	static const int iir_taps = sizeof(a);
	double input_hist[sizeof(a)] = { 0 };
	double output_hist[sizeof(a)] = { 0 };

	/* Generate data and print original data to file*/

	FILE* in_file = fopen(in_filename, "w");

	fprintf(in_file, "Time,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++)
	{
		t[i] = (double)(i) / SAMPLING_FREQUENCY;
		signal_in[i] = SINE_AMPL_1 * sin(pi2 * SINE_FREQ_1 * t[i])
			+ SINE_AMPL_2 * sin(pi2 * SINE_FREQ_2 * t[i]);
		fprintf(in_file, "%f,%f\n", t[i], signal_in[i]);
	}

	fclose(in_file);

	/* Perform the IIR filtering algorithm */

	for (int c = 0; c < DATA_SIZE; c++)
	{
		input_hist[c % iir_taps] = signal_in[c];
		for (int i = 0; i < (iir_taps - 1); i++)
		{
			signal_out[c] = signal_out[c] + b[i] * input_hist[(c - i + iir_taps) % iir_taps];
		}
		for (int i = 1; i < (iir_taps - 1); i++)
		{
			signal_out[c] = signal_out[c] - a[i] * output_hist[(c - i + iir_taps) % iir_taps];
		}
		signal_out[c] = signal_out[c] / a[0];
		output_hist[c % iir_taps] = signal_out[c];
	}



	/* Print out the data to file */

	FILE* out_file = fopen(out_filename, "w");
	fprintf(out_file, "t,Amplitude\n");
	for (int i = 0; i < DATA_SIZE; i++)
	{
		fprintf(out_file, "%f,%f\n", t[i], signal_out[i]);
	}

	fclose(out_file);

	return 0;
}

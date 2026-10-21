#include "../../cpp_wrapper_complex.h"
#include <cmath>
#include <cstdio>

using namespace std;

#define SAMPLE_RATE 8000
#define DATA_LENGTH 64
/* The tone is mixed down to 250 Hz by an oscillator 1 kHz below it */
#define TONE_FREQ 1250.0
#define OSC_FREQ 1000.0
#define TONE_AMPLITUDE 0.5
#define TONE_PHASE 0.3
/* The int32 samples are Q31, from -1.0 to just under 1.0 */
#define Q31_BITS 31
#define Q31_SCALE 2147483648.0

static int32_t to_q31(double v) {
	return (int32_t)llround(fmin(v * Q31_SCALE, Q31_SCALE - 1.0));
}

int main() {
	/* Single numbers, with the operators for float */
	complex_t<float> a = {3.0f, 4.0f};
	complex_t<float> b = {1.0f, -2.0f};
	complex_t<float> sum = a + b;
	complex_t<float> quotient = a / b;
	polar_t<float> p = complex_to_polar(a);
	complex_t<float> back = complex_from_polar(p);
	printf("a + b = %g%+gj, a / b = %g%+gj\n", sum.real, sum.imag,
		   quotient.real, quotient.imag);
	printf("|a| = %g, phase %.5f rad, from polar %g%+gj\n\n", p.magnitude,
		   p.phase, back.real, back.imag);

	/* A complex tone, and an oscillator to shift it down in frequency, in
	 * float and Q31 */
	vector<complex_t<float>> tone(DATA_LENGTH), osc(DATA_LENGTH);
	vector<complex_t<int32_t>> tone_q31(DATA_LENGTH), osc_q31(DATA_LENGTH);
	for (unsigned int n = 0; n < DATA_LENGTH; n++) {
		double tone_angle = 2.0 * M_PI * TONE_FREQ * n / SAMPLE_RATE + TONE_PHASE;
		double osc_angle = -2.0 * M_PI * OSC_FREQ * n / SAMPLE_RATE;
		tone[n] = {(float)(TONE_AMPLITUDE * cos(tone_angle)),
				   (float)(TONE_AMPLITUDE * sin(tone_angle))};
		osc[n] = {(float)cos(osc_angle), (float)sin(osc_angle)};
		tone_q31[n] = {to_q31(TONE_AMPLITUDE * cos(tone_angle)),
					   to_q31(TONE_AMPLITUDE * sin(tone_angle))};
		osc_q31[n] = {to_q31(cos(osc_angle)), to_q31(sin(osc_angle))};
	}

	/* Mix in place, the integer multiply takes the fractional bits */
	complex_multiply(tone, osc, tone);
	complex_multiply(tone_q31, osc_q31, tone_q31, Q31_BITS);

	/* The mixed tone should keep its amplitude and turn by 2 pi 250 / 8000
	 * radians a sample. The int32 phase is float */
	vector<float> magnitude, phase;
	vector<int32_t> magnitude_q31;
	vector<float> phase_q31;
	complex_magnitude(tone, magnitude);
	complex_phase(tone, phase);
	complex_magnitude(tone_q31, magnitude_q31);
	complex_phase(tone_q31, phase_q31);

	printf("Sample | Magnitude          | Phase (rad)\n");
	printf("       | float    int32     | float     int32     expected\n");
	for (unsigned int n = 0; n < DATA_LENGTH; n += 8) {
		double expected = remainder(
			2.0 * M_PI * (TONE_FREQ - OSC_FREQ) * n / SAMPLE_RATE + TONE_PHASE,
			2.0 * M_PI);
		printf("%6u | %.6f %.6f  | %+.6f %+.6f %+.6f\n", n, magnitude[n],
			   magnitude_q31[n] / Q31_SCALE, phase[n], phase_q31[n], expected);
	}
	return 0;
}

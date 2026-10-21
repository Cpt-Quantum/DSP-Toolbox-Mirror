#ifndef WAVEFORM_GEN_SHARED_H
#define WAVEFORM_GEN_SHARED_H

/* Enum type for setting what function should be used for a wave used as a */
/* superposition. Used in the wave_properties_*_t structs. The periodic */
/* functions all have a peak of +/- amplitude, and the square, triangle, */
/* sawtooth and chirp match scipy.signal's square, sawtooth(width=0.5), */
/* sawtooth and chirp(method="linear"). */
typedef enum {
	/* amplitude * sin(2 pi (frequency * t + phase)) */
	FUNCTION_SINE,
	/* amplitude * cos(2 pi (frequency * t + phase)) */
	FUNCTION_COS,
	/* +amplitude for the first duty_cycle of each period, then -amplitude */
	FUNCTION_SQUARE,
	/* Rises from -amplitude to +amplitude over the first half of each period,
	 * then falls back */
	FUNCTION_TRIANGLE,
	/* Rises from -amplitude to +amplitude over each period */
	FUNCTION_SAWTOOTH,
	/* Linear frequency sweep, amplitude * cos(2 pi (frequency * t +
	 * sweep_rate * t^2 / 2 + phase)), starting at frequency and changing by
	 * sweep_rate Hz per second */
	FUNCTION_CHIRP,
	/* amplitude at the first sample at or after delay seconds, 0 elsewhere */
	FUNCTION_IMPULSE,
	/* amplitude from the first sample at or after delay seconds, 0 before */
	FUNCTION_STEP,
} WAVEFORM_FUNC_E;

#endif /* WAVEFORM_GEN_SHARED_H */

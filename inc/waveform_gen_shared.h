#ifndef WAVEFORM_GEN_SHARED_H
#define WAVEFORM_GEN_SHARED_H

/* Enum type for setting what function should be used for a wave used as a */
/* superposition. Used in the wave_properties_*_t structs. */
typedef enum {
	FUNCTION_SINE,
	FUNCTION_COS,
} WAVEFORM_FUNC_E;

#endif /* WAVEFORM_GEN_SHARED_H */

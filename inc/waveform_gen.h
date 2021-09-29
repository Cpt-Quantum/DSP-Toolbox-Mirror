#ifndef WAVEFORM_GEN
#define WAVEFORM_GEN

#include <stdint.h>

#include "complex.h"

/* Enum type for setting what function should be used for a wave used as a */
/* superposition. Used in the wave_properties_t struct. */
typedef enum {
	FUNCTION_SINE = 0,
	FUNCTION_COS = 1,
	FUNCTION_SINE_IMAG = 2,
	FUNCTION_COS_IMAG = 3,
} HARMONIC_FUNC_E;

/* Struct type for initialising an individual superposition wave as part of a */
/* full waveform. Multiple structs can be passed of any number 0 to 255 to */
/* wave_settings_t struct. They will all be added into the waveform when the */
/* init_waveform() function is called. */
/* amplitude: Sets the superposition wave's amplitude */
/* offset: Sets the superposition wave's DC offset */
/* frequency: Sets the superposition wave's frequency */
/* function_type: Sets the function to use for the superposition wave */
typedef struct {
	const float amplitude;
	const float offset;
	const float frequency;
	const float phase;
	const HARMONIC_FUNC_E function_type;
} wave_properties_t;

/* Struct type for initialising a full waveform as a collection of individual */
/* superposition waves set by the *superposition member. */
/* fs: Sampling frequency of the overall waveform */
/* data_length: The length of the arrays being passed to initialise */
/* dc_offset: Overall DC offset of the signal (e.g the signal is biased) */
/* *superposition: Pointer to an initialised object describing the properties */
/* of each superposition wave. */
/* num_superpositions: Number of indiviual superposition waves in the */
/* resulting final waveform. */
typedef struct {
	const uint32_t fs;
	const uint32_t data_length;
	const float dc_offset;
	const wave_properties_t* superposition;
	const uint8_t num_superpositions;
} wave_settings_t;

/* Struct type for initialising a full waveform as a collection of individual */
/* superposition waves set by the *superposition member. */
/* fs: Sampling frequency of the overall waveform */
/* data_length: The length of the arrays being passed to initialise */
/* dc_offset: Overall DC offset of the signal (e.g the signal is biased), has both real and */
/* imaginary components. */
/* *superposition: Pointer to an initialised object describing the properties */
/* of each superposition wave. */
/* num_superpositions: Number of indiviual superposition waves in the */
/* resulting final waveform. */
typedef struct {
	const uint32_t fs;
	const uint32_t data_length;
	const complex_t dc_offset;
	const float dc_offset_imag;
	const wave_properties_t* superposition;
	const uint8_t num_superpositions;
} wave_settings_complex_t;

/* Function for generating time information */
/* t: The buffer to set the time of */
/* length: The length of the time buffer */
/* sample_freq: The sampling frequency for the waveform */
void gen_time(float* t, uint32_t length, uint32_t sample_freq);

/* Function for generation frequency information */
/* f: The buffer the frequencies are placed into */
/* length: The length of the frequency buffer */
/* sample_freq: The sampling frequency of the waveform */
void gen_freq(float* f, uint32_t length, uint32_t sample_freq);

/* The following functions add sine and cosine waveforms to a pre-existing    */
/* data buffer. This function is defined for use with purely real data.       */
/* x: The x buffer storing time */
/* y: The data buffer to add the sine/cosine wave to */
/* wave_properties: The struct containing all desired properties for the */
/* waveform to be added. */
void add_sin(float* x, float* y,
	wave_properties_t wave_properties, uint32_t length);
void add_cos(float* x, float* y,
	wave_properties_t wave_properties, uint32_t length);

/* The following functions add sine and cosine waveforms to a pre-existing    */
/* data buffer. This function is defined for use with complex data.           */
/* The _real functions add the waveform to the real parts, whereas the _imag  */
/* functions add the waveform to the imaginary parts.                         */
/* x: The x buffer storing time */
/* y: The data buffer to add the sine/cosine wave to */
/* wave_properties: The struct containing all desired properties for the */
/* waveform to be added. */
void add_sin_real(float* x, complex_t* y,
	wave_properties_t wave_properties, uint32_t length);
void add_cos_real(float* x, complex_t* y,
	wave_properties_t wave_properties, uint32_t length);
void add_sin_imag(float* x, complex_t* y,
	wave_properties_t wave_properties, uint32_t length);
void add_cos_imag(float* x, complex_t* y,
	wave_properties_t wave_properties, uint32_t length);

/* Initialises the waveform based on the pass wave_settings */
/* wave_settings: Struct containing all of the wave settings required for */
/* initialisation. */
/* *t: pointer to the time array */
/* *x: pointer to the waveform array */
void init_waveform(wave_settings_t wave_settings, float* t, float* x);

/* Initialises a complex waveform based on the passed wave_settings */
/* wave_settings: Struct containing all of the wave settings required for */
/* initialisation. */
/* *t: pointer to the time array */
/* *x: pointer to the waveform array */
void init_waveform_complex(wave_settings_complex_t wave_settings, float* t, complex_t* x);


#endif //WAVEFORM_GEN

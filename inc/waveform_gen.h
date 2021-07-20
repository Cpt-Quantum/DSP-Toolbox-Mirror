#ifndef WAVEFORM_GEN
#define WAVEFORM_GEN

#include <stdint.h>

#include "complex.h"

/* Enum type for setting what function should be used for a wave used as a */
/* superposition. Used in the wave_properties_t struct. */
typedef enum {
        FUNCTION_SINE = 0,
        FUNCTION_COS  = 1
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
/* *superposition: Pointer to an initialised object describing the properties */
/* of each superposition wave. */
/* num_superpositions: Number of indiviual superposition waves in the */
/* resulting final waveform. */
typedef struct {
        const uint32_t fs;
        const uint32_t data_length;
        const wave_properties_t *superposition;
        const uint8_t num_superpositions;
} wave_settings_t;

/* Function for generating time information */
/* t: The buffer to set the time of */
/* length: The length of the time buffer */
/* sample_freq: The sampling frequency for the waveform */
void gen_time(float *t, uint32_t length, uint32_t sample_freq);

/* Function for generation frequency information */
/* f: The buffer the frequencies are placed into */
/* length: The length of the frequency buffer */
/* sample_freq: The sampling frequency of the waveform */
void gen_freq(float *f, uint32_t length, uint32_t sample_freq);

/* The following functions add sine and cosine waveforms to a pre-existing    */
/* data buffer. This function is defined for use with purely real data.       */
/* x: The x buffer storing time */
/* y: The data buffer to add the sine/cosine wave to */
/* length: The length of the y data buffer */
/* frequency: The frequency of the waveform */
/* amplitude: The amplitude of the sine/cosine wave */
/* phase: The phase of the sine/cosine wave */
void add_sin(float *x, float *y, uint32_t length, uint32_t frequency,
				float amplitude, float offset, float phase);
void add_cos(float *x, float *y, uint32_t length, uint32_t frequenct,
				float amplitude, float offset, float phase);

/* The following functions add sine and cosine waveforms to a pre-existing    */
/* data buffer. This function is defined for use with complex data.           */
/* The _real functions add the waveform to the real parts, whereas the _imag  */
/* functions add the waveform to the imaginary parts.                         */
/* x: The x buffer storing time */
/* y: The data buffer to add the sine/cosine wave to */
/* length: The length of the y data buffer */
/* frequency: The frequency of the waveform */
/* amplitude: The amplitude of the sine/cosine wave */
/* phase: The phase of the sine/cosine wave */
void add_sin_real(float *x, complex_t *y, uint32_t length, uint32_t frequency,
					float amplitude, float offset, float phase);
void add_cos_real(float *x, complex_t *y, uint32_t length, uint32_t frequenct,
					float amplitude, float offset, float phase);
void add_sin_imag(float *x, complex_t *y, uint32_t length, uint32_t frequency,
					float amplitude, float offset, float phase);
void add_cos_imag(float *x, complex_t *y, uint32_t length, uint32_t frequenct,
					float amplitude, float offset, float phase);

/* Initialises the waveform based on the pass wave_settings */
/* wave_settings: Struct containing all of the wave settings required for */
/* initialisation. */
/* *t: pointer to the time array */
/* *x: pointer to the waveform array */
void init_waveform(wave_settings_t wave_settings, float *t, float *x);

#endif //WAVEFORM_GEN

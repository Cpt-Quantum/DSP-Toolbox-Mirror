#ifndef WAVEFORM_GEN
#define WAVEFORM_GEN

#ifdef __cplusplus
extern "C" {
#endif

#include "complex.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef DOUBLE_EN
#define FLOAT double
#else
#define FLOAT float
#endif

/* Enum type for setting what function should be used for a wave used as a */
/* superposition. Used in the wave_properties_t struct. */
typedef enum {
	FUNCTION_SINE,
	FUNCTION_COS,
} WAVEFORM_FUNC_E;

/* Struct type for initialising an individual superposition wave as part of a */
/* full waveform. Multiple structs can be passed of any number 0 to 255 to */
/* wave_settings_t struct. They will all be added into the waveform when the */
/* init_waveform() function is called. */
/* amplitude: Sets the superposition wave's amplitude */
/* offset: Sets the superposition wave's DC offset */
/* frequency: Sets the superposition wave's frequency */
/* function_type: Sets the function to use for the superposition wave */
typedef struct {
	float amplitude;
	float offset;
	float frequency;
	float phase;
	WAVEFORM_FUNC_E function_type;
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
	uint32_t fs;
	uint32_t data_length;
	float dc_offset;
	wave_properties_t *superposition;
	uint8_t num_superpositions;
} wave_settings_t;

/* Struct type for initialising a full waveform as a collection of individual */
/* superposition waves set by the *superposition member. */
/* fs: Sampling frequency of the overall waveform */
/* data_length: The length of the arrays being passed to initialise */
/* dc_offset: Overall DC offset of the signal (e.g the signal is biased), has
 * both real and */
/* imaginary components. */
/* *superposition: Pointer to an initialised object describing the properties */
/* of each superposition wave. */
/* num_superpositions: Number of indiviual superposition waves in the */
/* resulting final waveform. */
typedef struct {
	uint32_t fs;
	uint32_t data_length;
	complex_t dc_offset;
	wave_properties_t *superposition_real;
	wave_properties_t *superposition_imag;
	uint8_t num_superpositions;
} wave_settings_complex_t;

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
/* t: The t buffer storing time */
/* x: The data buffer to add the sine/cosine wave to */
/* wave_properties: The struct containing all desired properties for the */
/* waveform to be added. */
void add_sin(float *t, float *x, wave_properties_t wave_properties,
			 uint32_t length);
void add_cos(float *t, float *x, wave_properties_t wave_properties,
			 uint32_t length);

/* The following functions add sine and cosine waveforms to a pre-existing    */
/* data buffer. This function is defined for use with complex data.           */
/* t: The t buffer storing time */
/* x: The data buffer to add the sine/cosine wave to */
/* write_real: Sets whether the function adds to the real part */
/* write_imag: Sets whether the function adds to the imaginary part */
/* wave_properties: The struct containing all desired properties for the */
/* waveform to be added. */
void add_sin_complex(float *t, complex_t *x, bool write_real, bool write_imag,
					 wave_properties_t wave_properties, uint32_t length);
void add_cos_complex(float *t, complex_t *x, bool write_real, bool write_imag,
					 wave_properties_t wave_properties, uint32_t length);

/* The following functions act as look up tables to call the correct function
 * for adding a wave to the waveform based on the passed wave_properties object.
 */
/* wave_properties: The waveform characteristics struct */
/* t: The t buffer storing time */
/* x: The data buffer to add the sine/cosine wave to */
/* data_length: The length of the t and x arrays */
/* Complex function only parameters: */
/* write_real: Sets whether the function adds to the real part */
/* write_imag: Sets whether the function adds to the imaginary part */
void waveform_add_superposition(wave_properties_t wave_properties, float *t,
								float *x, uint32_t data_length);
void waveform_add_superposition_complex(wave_properties_t wave_properties,
										float *t, complex_t *x, bool write_real,
										bool write_imag, int32_t data_length);

/* Initialises the waveform based on the pass wave_settings */
/* wave_settings: Struct containing all of the wave settings required for */
/* initialisation. */
/* *t: pointer to the time array */
/* *x: pointer to the waveform array */
void init_waveform(wave_settings_t wave_settings, float *t, float *x);

/* Initialises a complex waveform based on the passed wave_settings */
/* wave_settings: Struct containing all of the wave settings required for */
/* initialisation. */
/* *t: pointer to the time array */
/* *t: pointer to the waveform array */
void init_waveform_complex(wave_settings_complex_t wave_settings, float *t,
						   complex_t *x);

#ifdef __cplusplus
}
#endif

#endif // WAVEFORM_GEN

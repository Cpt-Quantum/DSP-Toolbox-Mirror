# DSP Toolbox C API Reference

This document describes the available C functions and data structures generated from the Jinja2 templates. The API is available for multiple numerical types, including `float`, `double`, `int32_t`, and `int64_t`.

In the descriptions below, `{{suffix}}` represents the type suffix (e.g., `float`, `double`, `int32`, `int64`) and `{{type}}` represents the actual C type (e.g., `float`, `double`, `int32_t`, `int64_t`).

---

## Table of Contents
- [Complex Math](#complex-math)
- [FIR Filters](#fir-filters)
- [IIR Filters](#iir-filters)
- [JSON Initialization](#json-initialization)
- [Waveform Generation](#waveform-generation)
- [Spectrum Generation](#spectrum-generation)

---

## Complex Math

Structures and functions for complex number arithmetic in `complex_{{suffix}}.h`.

### Data Structures

```c
typedef struct {
    {{type}} real;
    {{type}} imag;
} complex_{{suffix}}_t;
```

### Functions

| Function | Description |
| :--- | :--- |
| `complex_multiply_{{suffix}}(x, y)` | Multiplies two complex numbers. |
| `complex_add_{{suffix}}(x, y)` | Adds two complex numbers. |
| `complex_conjugate_{{suffix}}(x)` | Returns the complex conjugate of `x`. |
| `complex_magnitude_{{suffix}}(x)` | Returns the magnitude (absolute value) of `x`. |

---

## FIR Filters

Finite Impulse Response filter implementation in `fir_{{suffix}}.h`.

The FIR and Direct Form IIR filters keep a linear history buffer per filter, holding the previous `taps - 1` samples followed by space for a block of new samples. Input is processed in blocks of up to `FILTER_BLOCK_SIZE` samples (default 128, defined in `filter_shared.h`), with several outputs calculated together so they can be vectorised. Each output's terms are still summed in coefficient order. Any input `length` is supported. History buffers must be zeroed and hold `FILTER_STATE_SIZE(taps)` elements. If `FILTER_BLOCK_SIZE` is overridden, it must be defined the same way for every file in the build.

#### Build configuration (`filter_shared.h`)

| Macro | Default | Description |
| :--- | :--- | :--- |
| `FILTER_BLOCK_SIZE` | `128` | Maximum new samples processed per block. Sets the history buffer size, so it must match across the whole build. |
| `FILTER_OUTPUT_GROUP` | `32` | Outputs calculated together. Large for wide SIMD; a smaller group (e.g. 4) may suit scalar FPU cores. Affects speed only, never results. |

#### Generation options (copy functions)

The FIR and IIR filters copy new samples into, and shift, their history buffers. The copy functions are chosen when the sources are generated (`python scripts/generate_sources.py --help`):

| Option | Description |
| :--- | :--- |
| *(default)* | `memcpy`/`memmove` from the C standard library. |
| `--no-stdlib` | Plain copy loops written into the generated code, for targets without a C standard library (e.g. MCUs). The FIR and IIR sources then only include freestanding headers (`<stdint.h>`, `<stddef.h>`). |
| `--memcpy FUNC` / `--memmove FUNC` | User functions with the `memcpy`/`memmove` signatures (e.g. DMA or vendor optimised copies). Either can be given alone, and the other falls back to the default (or the copy loops with `--no-stdlib`). The `--memmove` function must handle overlapping regions where the destination comes first. |
| `--copy-header HEADER` | Header declaring the `--memcpy`/`--memmove` functions, included by the generated filter sources. |

The JSON initialisation functions require a hosted environment (cJSON, `malloc`, `printf`) regardless of these options.

### Data Structures

```c
struct fir_{{suffix}}_t {
    {{type}} *b;           // Filter coefficients
    {{type}} *in_buff;     // Input history, FILTER_STATE_SIZE(taps) elements
    uint16_t taps;         // Number of filter taps
    uint8_t decimation_phase; // Input samples until the next decimated output
};
```

### Functions

| Function | Description |
| :--- | :--- |
| `fir_filter_{{suffix}}(data_in, data_out, filter, length)` | Processes a buffer of data through the FIR filter. |
| `fir_decimate_{{suffix}}(data_in, data_out, filter, length, rate)` | Filters and decimates the input data by the specified `decimation_rate`, returning the number of output samples written. The decimation phase is kept between calls, so `length` does not need to be a multiple of the rate; `data_out` must hold at least `ceil(length / rate)` samples. |

---

## IIR Filters

Infinite Impulse Response filter implementation (Direct Form and SOS) in `iir_{{suffix}}.h`.

### Data Structures

**Direct Form:**

```c
struct iir_{{suffix}}_t {
    {{type}} *a;           // Feedback coefficients
    {{type}} *b;           // Feedforward coefficients
    {{type}} *in_buff;     // Input history, FILTER_STATE_SIZE(taps) elements
    {{type}} *out_buff;    // Output history, FILTER_STATE_SIZE(taps) elements
    uint16_t taps;         // Number of coefficients
};
```

**Second Order Sections (SOS / Biquad):**

```c
struct biquad_section_{{suffix}}_t {
    {{type}} a[3], b[3];   // Section coefficients
    {{type}} x1, x2, y1, y2; // Previous two inputs and outputs
};

struct sos_filter_{{suffix}}_t {
    struct biquad_section_{{suffix}}_t *biquad_section;
    uint16_t num_sections;
    {{type}} gain;         // Overall filter gain
};
```

### Functions

| Function | Description |
| :--- | :--- |
| `iir_filter_{{suffix}}(data_in, data_out, filter, length)` | Processes data through a Direct Form IIR filter. |
| `sos_filter_{{suffix}}(data_in, data_out, sos_filter, length)` | Processes data through an SOS (Biquad) IIR filter. Any `length` is supported, and `data_in` may be the same buffer as `data_out`. |

---

## JSON Initialization

Functions for initializing filter structures from JSON (using cJSON) in `json_to_filt_{{suffix}}.h`.

| Function | Description |
| :--- | :--- |
| `get_sample_rate_from_json_{{suffix}}(node)` | Extracts sample rate from a JSON node. |
| `init_fir_from_json_{{suffix}}(filt, node)` | Initializes an FIR filter struct from JSON. |
| `init_iir_from_json_{{suffix}}(filt, node)` | Initializes an IIR filter struct from JSON. |
| `init_sos_from_json_{{suffix}}(filt, node)` | Initializes an SOS filter struct from JSON. |
| `free_fir_{{suffix}}(filt)` | Frees memory allocated for an FIR filter's buffers. |
| `free_iir_{{suffix}}(filt)` | Frees memory allocated for an IIR filter's buffers. |
| `free_sos_{{suffix}}(filt)` | Frees memory allocated for an SOS filter's sections. |

---

## Waveform Generation

Tools for creating signals and time/frequency vectors in `waveform_gen_{{suffix}}.h`.

### Data Structures

| Structure | Description |
| :--- | :--- |
| `wave_properties_{{suffix}}_t` | Defines a single superposition component (Amp, Freq, Phase, etc.). |
| `wave_settings_{{suffix}}_t` | Settings for generating a real-valued composite waveform. |
| `wave_settings_complex_{{suffix}}_t` | Settings for generating a complex-valued composite waveform. |

### Generation Functions

| Function | Description |
| :--- | :--- |
| `gen_time_{{suffix}}(t, len, fs)` | Fills `t` with time values based on sample frequency. |
| `gen_freq_{{suffix}}(f, len, fs)` | Fills `f` with frequency bins. |
| `add_sin_{{suffix}}(t, x, props, len)` | Adds a sine component to buffer `x`. |
| `add_cos_{{suffix}}(t, x, props, len)` | Adds a cosine component to buffer `x`. |
| `init_waveform_{{suffix}}(settings, t, x)` | Fully initializes a waveform buffer from settings. |
| `init_waveform_complex_{{suffix}}(settings, t, x)` | Fully initializes a complex waveform buffer from settings. |

### Post-Processing

| Function | Description |
| :--- | :--- |
| `scale_waveform_{{suffix}}(x, factor, len)` | Multiplies buffer by a scalar. |
| `divide_waveform_{{suffix}}(x, factor, len)` | Divides buffer by a scalar. |
| `scale_waveform_complex_{{suffix}}(x, factor, len)` | Scales a complex buffer. |
| `divide_waveform_complex_{{suffix}}(x, factor, len)` | Divides a complex buffer. |
| `mask_waveform_{{suffix}}(x, bit_width, len)` | Limits (saturates) buffer values to the range of a signed integer of `bit_width`. |
| `mask_waveform_complex_{{suffix}}(x, bit_width, len)` | Limits (saturates) complex buffer values to the range of a signed integer of `bit_width`. |

---

## Spectrum Generation

Automated test signal generation based on frequency profiles in `spectrum_gen_{{suffix}}.h`.

### Functions

| Function | Description |
| :--- | :--- |
| `generate_spectrum_waveform_{{suffix}}(...)` | Generates a complex waveform containing a frequency sweep or noise based on a profile (Flat, Low Decay, High Decay, Bandpass, etc.). |

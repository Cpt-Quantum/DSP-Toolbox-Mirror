# DSP Toolbox C API Reference

This document describes the available C functions and data structures generated from the Jinja2 templates. The API is available for multiple numerical types, including `float`, `double`, `int32_t`, and `int64_t`.

In the descriptions below, `{{suffix}}` represents the type suffix (e.g., `float`, `double`, `int32`, `int64`) and `{{type}}` represents the actual C type (e.g., `float`, `double`, `int32_t`, `int64_t`).

---

## Table of Contents

- [Constants](#constants)
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

### Data Structures

```c
struct fir_{{suffix}}_t {
    {{type}} *b;           // Filter coefficients
    {{type}} *in_buff;     // Input delay line buffer
    uint16_t taps;         // Number of filter taps
    uint16_t cycle;        // Internal circular buffer index
};
```

### Functions

| Function | Description |
| :--- | :--- |
| `fir_filter_{{suffix}}(data_in, data_out, filter, length)` | Processes a buffer of data through the FIR filter. |
| `fir_decimate_{{suffix}}(data_in, data_out, filter, length, rate)` | Filters and decimates the input data by the specified `decimation_rate`. |

---

## IIR Filters

Infinite Impulse Response filter implementation (Direct Form and SOS) in `iir_{{suffix}}.h`.

### Data Structures

**Direct Form:**

```c
struct iir_{{suffix}}_t {
    {{type}} *a;           // Feedback coefficients
    {{type}} *b;           // Feedforward coefficients
    {{type}} *in_buff;     // Input delay line
    {{type}} *out_buff;    // Output delay line
    uint16_t taps;         // Number of coefficients
    uint16_t cycle;        // Internal index
};
```

**Second Order Sections (SOS / Biquad):**

```c
struct biquad_section_{{suffix}}_t {
    {{type}} a[3], b[3];   // Section coefficients
    {{type}} in_buff[3], out_buff[3]; // Section delay lines
    uint16_t cycle;        // Internal index
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
| `sos_filter_{{suffix}}(data_in, data_out, sos_filter, length)` | Processes data through an SOS (Biquad) IIR filter. |

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

---

## Spectrum Generation

Automated test signal generation based on frequency profiles in `spectrum_gen_{{suffix}}.h`.

### Functions

| Function | Description |
| :--- | :--- |
| `generate_spectrum_waveform_{{suffix}}(...)` | Generates a complex waveform containing a frequency sweep or noise based on a profile (Flat, Low Decay, High Decay, Bandpass, etc.). |

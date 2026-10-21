# DSP Toolbox C API Reference

This document describes the available C functions and data structures generated from the Jinja2 templates. The API is available for multiple numerical types, including `float`, `double`, `int32_t`, and `int64_t`.

In the descriptions below, `{{suffix}}` represents the type suffix (e.g., `float`, `double`, `int32`, `int64`) and `{{type}}` represents the actual C type (e.g., `float`, `double`, `int32_t`, `int64_t`).

---

## Table of Contents
- [Complex Math](#complex-math)
- [FIR Filters](#fir-filters)
- [IIR Filters](#iir-filters)
- [JSON Initialization](#json-initialization)
- [Averaging](#averaging)
- [Waveform Generation](#waveform-generation)
- [Spectrum Generation](#spectrum-generation)
- [FFT](#fft)

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

#### Integer filters (fixed point)

The `int32` and `int64` filters (FIR, Direct Form IIR and SOS) use Q format coefficients. Each filter struct has a `frac_bits` field, and a coefficient `c` has the value `c / 2^frac_bits`. For example, 0.25 in Q1.30 (`frac_bits = 30`) is `1 << 28`.

| | `int32` | `int64` |
| :--- | :--- | :--- |
| Accumulator | `int64_t` | `int64_t` |
| Output | Rounded to nearest (halves up), shifted down by `frac_bits`, saturated to `INT32_MIN`/`INT32_MAX` | Rounded to nearest (halves up), shifted down by `frac_bits` |
| `frac_bits` (FIR) | 0 to 31 | 0 to 62 |
| `frac_bits` (IIR/SOS) | 0 to 30, so 1.0 can be represented | 0 to 62 |

- Each output is rounded once, after all of its terms are summed. For the IIR filters this includes the feedforward and feedback terms together.
- The IIR filters don't divide by `a[0]`. The shift by `frac_bits` takes its place, so the coefficients must be normalised with `a[0] = 1 << frac_bits`. SciPy's designs are normalised already. The same applies to each SOS section's `a[0]`, and the SOS `gain` is also in Q format.
- The sum must not overflow the accumulator, so choose the Q format to leave headroom. For full-scale 32-bit input, the absolute coefficients (as integers) must add up to less than 2^32. An `int64` filter has no wider accumulator, so its input must use far fewer than 64 bits (e.g. 16 or 32-bit samples stored in `int64_t`).
- Negative values are shifted right arithmetically, as on GCC, Clang and the common embedded compilers.
- On a Cortex-M4 the `int32` filters compile to `SMLAL` (single-cycle 32x32+64 multiply-accumulate).

The float and double filters are unchanged and have no `frac_bits` field.

To quantise coefficients, pass `fixed_point_format=(total_bits, frac_bits)` to the design functions in `scripts/filter_design_wrapper.py`. The coefficients are then rounded to integers and `frac_bits` is exported to the JSON with them. A `ValueError` is raised if a coefficient doesn't fit the format, rather than clipping it and changing the response.

The example in `filter/main.c` runs `int32` copies of each filter alongside the float ones, on the same input. `make` in `filter/` plots both, and `scripts/compare_filters.py` prints each one's SNR against a double precision SciPy reference.

### Data Structures

```c
struct fir_{{suffix}}_t {
    {{type}} *b;           // Filter coefficients
    {{type}} *in_buff;     // Input history, FILTER_STATE_SIZE(taps) elements
    uint16_t taps;         // Number of filter taps
    uint8_t decimation_phase; // Input samples until the next decimated output
    uint8_t frac_bits;     // Integer types only, Q format fractional bits
};
```

### Functions

| Function | Description |
| :--- | :--- |
| `fir_filter_{{suffix}}(data_in, data_out, filter, length)` | Processes a buffer of data through the FIR filter. |
| `fir_decimate_{{suffix}}(data_in, data_out, filter, length, rate)` | Filters and decimates the input data by the specified `decimation_rate`, returning the number of output samples written. The decimation phase is kept between calls, so `length` does not need to be a multiple of the rate; `data_out` must hold at least `ceil(length / rate)` samples. |

---

## IIR Filters

Infinite Impulse Response filter implementation (Direct Form and SOS) in `iir_{{suffix}}.h`. The integer types use Q format coefficients, see [Integer filters](#integer-filters-fixed-point).

### Data Structures

**Direct Form:**

```c
struct iir_{{suffix}}_t {
    {{type}} *a;           // Feedback coefficients
    {{type}} *b;           // Feedforward coefficients
    {{type}} *in_buff;     // Input history, FILTER_STATE_SIZE(taps) elements
    {{type}} *out_buff;    // Output history, FILTER_STATE_SIZE(taps) elements
    uint16_t taps;         // Number of coefficients
    uint8_t frac_bits;     // Integer types only, Q format fractional bits
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
    uint8_t frac_bits;     // Integer types only, Q format fractional bits
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

The integer loaders also read `frac_bits` from the filter's JSON object, as written by `scripts/filter_design_wrapper.py`. They warn if it is missing, in which case the coefficients are treated as integers. They also warn if an IIR `a[0]` isn't `1 << frac_bits`. Coefficients pass through a JSON double, so `int64` coefficients are exact only up to 53 bits.

---

## Averaging

Block averages and a moving average in `average/avg_{{suffix}}.h`, for every type. They only need freestanding headers, so they build without a C standard library.

The integer types sum in `int64_t` and round each average to the nearest value, with halves away from zero. The sums must not overflow `int64_t`. For `power_average_{{suffix}}()`, the number of samples times the largest squared sample must be below 2^63. For example, that allows up to 2^32 samples of 16-bit data, or 2^16 samples of full-scale 24-bit data. The power of an `int32` signal is returned as `int64_t`, as the squares don't fit in `int32_t`.

### Data Structures

```c
struct moving_avg_{{suffix}}_t {
    {{type}} *buff;        // Window of the most recent samples, provided by the caller
    uint32_t length;       // Window length
    uint32_t head;         // Index of the oldest sample, which the next sample replaces
    uint32_t count;        // Samples received, up to length
    {{acc_type}} total;    // Sum of the samples in the window (int64_t for the integer types)
};
```

### Functions

| Function | Description |
| :--- | :--- |
| `average_{{suffix}}(data_in, length)` | Mean of `length` samples, 0 if `length` is 0. |
| `power_average_{{suffix}}(data_in, length)` | Mean of the squares of `length` samples, 0 if `length` is 0. Returns `{{acc_type}}`. |
| `moving_avg_init_{{suffix}}(avg, buff, length)` | Initialises `avg` with an empty window of `length` samples (at least 1) held in `buff`, which must outlive `avg`. |
| `moving_average_{{suffix}}(avg, new_samp)` | Adds a sample to the window and returns the new average. Until the window is full, this is the average of the samples received so far. |
| `moving_average_block_{{suffix}}(avg, data_in, data_out, length)` | `moving_average_{{suffix}}()` for each of `length` samples. `data_in` may be the same buffer as `data_out`. |

The moving average keeps a running total, so each sample costs the same whatever the window length. For float and double, rounding errors in the running total would build up over time, so the total is summed again from the window each time it wraps. This costs one addition per sample on average.

### Example

`average/main.c` removes the DC offset from a set of tones in two ways: by subtracting block averages, and by subtracting a moving average of chunk averages. It then calculates the power of each 1ms. Both are run with float and int32 samples. Running `make` in `average/` builds and runs the example, then `scripts/plot_results.py` checks the results against numpy and plots them to `average/results/average.png`.

---

## Waveform Generation

Tools for creating signals and time/frequency vectors in `waveform_gen_{{suffix}}.h`.

### Data Structures

| Structure | Description |
| :--- | :--- |
| `wave_properties_{{suffix}}_t` | Defines a single superposition component: `amplitude`, `offset`, `frequency`, `phase` (a fraction of a period), `function_type`, and `duty_cycle`, `sweep_rate` and `delay` for the functions that use them. |
| `wave_settings_{{suffix}}_t` | Settings for generating a real-valued composite waveform. |
| `wave_settings_complex_{{suffix}}_t` | Settings for generating a complex-valued composite waveform. |

### Functions (`WAVEFORM_FUNC_E`)

Every function has a peak of `amplitude`, and `offset` is added to every sample. The square, triangle, sawtooth and chirp match `scipy.signal`'s functions of the same name.

| Function | Waveform | Extra members |
| :--- | :--- | :--- |
| `FUNCTION_SINE` | `sin(2π(frequency·t + phase))` | |
| `FUNCTION_COS` | `cos(2π(frequency·t + phase))` | |
| `FUNCTION_SQUARE` | +1 for the first `duty_cycle` of each period, then -1. Matches `signal.square`. | `duty_cycle`, 0 to 1, where 0 selects 0.5 so a zero initialised struct is symmetric |
| `FUNCTION_TRIANGLE` | Rises from -1 to +1 over the first half of each period, then falls back. Matches `signal.sawtooth(width=0.5)`. | |
| `FUNCTION_SAWTOOTH` | Rises from -1 to +1 over each period. Matches `signal.sawtooth`. | |
| `FUNCTION_CHIRP` | Linear sweep `cos(2π(frequency·t + sweep_rate·t²/2 + phase))`, starting at `frequency`. Matches `signal.chirp(method="linear")`. | `sweep_rate`, in Hz per second, negative to sweep down |
| `FUNCTION_IMPULSE` | 1 at the first sample at or after `delay`, 0 elsewhere. `t` must be increasing. | `delay`, in seconds |
| `FUNCTION_STEP` | 0 before `delay`, then 1 from the first sample at or after it. | `delay`, in seconds |

Members that a function doesn't use can be left as 0, so designated initialisers only need the ones in use:

```c
wave_properties_float_t waves[] = {
    {.amplitude = 1, .frequency = 50, .function_type = FUNCTION_SQUARE, .duty_cycle = 0.25f},
    {.amplitude = 1, .frequency = 10, .function_type = FUNCTION_CHIRP, .sweep_rate = 90},
    {.amplitude = 2, .function_type = FUNCTION_STEP, .delay = 0.5f},
};
```

### Integer waveforms

The waveforms are always generated in floating point, `wave_math_{{suffix}}_t`. For `float` and `double` that is the type itself. For `int32` and `int64`, the time and frequency buffers (`gen_time_{{suffix}}()`, `gen_freq_{{suffix}}()`) and every member of `wave_properties_{{suffix}}_t` are floating point too, and only the samples are integers. Each wave is rounded to the nearest integer (halves away from zero) and saturated to the type's range as it is added, so waves past full scale clip rather than wrap. `dc_offset` in the settings structs stays in the sample type.

This is for creating integer test signals, e.g. for the integer filters, on a CPU or an MCU with an FPU. The precision is chosen when the sources are generated:

| Option | Description |
| :--- | :--- |
| *(default)* | `int32` in float, `int64` in double. |
| `--int-waveform-precision double` | Both in double. `int32` waveforms are then exact to about 1 LSB at full scale. |
| `--int-waveform-precision float` | Both in float, for targets with only a single precision FPU. The phase is only accurate to float precision, so sine, chirp and the other periodic waves are out by about `1e-5` of the amplitude after tens of cycles (around 10000 LSB for an `int32` amplitude of `1e9`). Steps and impulses stay exact. |

`spectrum_gen` uses the same type for its time buffer and calculations. For the integer types it sums all of the frequency bins of each sample before rounding once, and rounds and saturates the noise in the same way.

`waveform_add_sample_{{suffix}}(x, value)` *(integer types only)* returns the sample `x` plus `value`, rounded and saturated as above, for adding other floating point signals to an integer waveform in the same way.

### Generation Functions

| Function | Description |
| :--- | :--- |
| `gen_time_{{suffix}}(t, len, fs)` | Fills `t` with time values based on sample frequency. |
| `gen_freq_{{suffix}}(f, len, fs)` | Fills `f` with frequency bins. |
| `add_sin_{{suffix}}(t, x, props, len)` | Adds a sine component to buffer `x`. |
| `add_cos_{{suffix}}(t, x, props, len)` | Adds a cosine component to buffer `x`. |
| `waveform_add_superposition_{{suffix}}(props, t, x, len)` | Adds the wave described by `props` to `x`, for any `WAVEFORM_FUNC_E`. |
| `waveform_add_superposition_complex_{{suffix}}(props, t, x, write_real, write_imag, len)` | The same for the real and/or imaginary parts of a complex buffer. |
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

### Example

`waveform/main.c` generates one wave of each of the non sinusoidal functions, and a superposition of several of them, writing them to `waveform/results/waveform_data.csv`. Running `make` in `waveform/` builds and runs it, then `scripts/plot_results.py` compares each wave against `scipy.signal` and plots them to `results/waveform.png`.

---

## Spectrum Generation

Automated test signal generation based on frequency profiles in `spectrum_gen_{{suffix}}.h`.

### Functions

| Function | Description |
| :--- | :--- |
| `generate_spectrum_waveform_{{suffix}}(t, x, data_length, sample_rate, num_steps, profile, base_amplitude, noise_variance, seed)` | Fills `x` with `num_steps` sine waves evenly spaced up to the Nyquist frequency, with amplitudes shaped by `profile` (Flat, Low Decay, High Decay, Bandpass, etc.) and scaled by `base_amplitude`, plus Gaussian noise with variance `noise_variance` (0 for none). Also fills `t` with the sample times. |

The sine waves repeat every `2 * num_steps` samples, so only one period is calculated and then repeated, which keeps generation fast and the phase exact for long waveforms. The noise comes from a built-in pseudo random generator seeded with `seed`, so the same seed always gives the same waveform. Use a different seed for each channel that should have independent noise.

---

## FFT

In place radix-2 Fourier transforms in `fft/fft_{{suffix}}.h`, generated for
`float` and `double` only, with a complex transform and a real input
transform. The status enum is shared between the types in
`fft/fft_shared.h`.

The forward transform uses a negative exponent and is unscaled. The inverse
uses a positive exponent and is scaled by `1 / n`, matching `numpy.fft.fft` and
`numpy.fft.ifft`. Output is in natural order, with bin `n - k` the negative
frequency matching bin `k`.

### Generation options (twiddles)

By default the twiddle factors are calculated once, during plan
initialisation, into a workspace provided by the caller. The transforms
themselves only use the FFT's own type.

| Option | Description |
| :--- | :--- |
| `--fft-twiddle-precision double` | *(default)* The float FFT calculates its twiddles in double and rounds them once to float. Plan initialisation is then the only place double is used. |
| `--fft-twiddle-precision float` | The float FFT calculates its twiddles in float, so no double arithmetic is used anywhere. For targets without a double FPU, where double is emulated in software. Accuracy is almost the same (relative RMS error 1.44e-7 against 1.43e-7 at N = 16384). |

The double FFT always calculates its twiddles in double.

The twiddles can instead be generated as a `const` table, so plans need no
workspace (the table can stay in flash) and no initialisation maths:

| Option | Description |
| :--- | :--- |
| `--fft-twiddle-table MAX_N` | Adds a table of the `MAX_N` point twiddles (a power of two), and the `fft_plan_init_table_{{suffix}}()` and `rfft_plan_init_table_{{suffix}}()` functions using it. Any power of two size up to `MAX_N` uses the same table, taking every `(MAX_N / n)`th entry. The table is `MAX_N / 2` complex values, e.g. 4 KiB of float for `MAX_N` = 1024. Its values are calculated in double and rounded once, and are bit for bit the same as the workspace twiddles with `--fft-twiddle-precision double`. The workspace functions are still generated. |
| `--fft-table-only` | With `--fft-twiddle-table`, leaves out the workspace functions and `dft_{{suffix}}()`, so the FFT doesn't include `<math.h>` or need the maths library. For MCUs, where the float FFT then uses only single precision arithmetic. |

```sh
python scripts/generate_sources.py --fft-twiddle-table 1024 --fft-table-only
```

### Data Structures

```c
typedef enum {
    FFT_OK,
    FFT_ERR_NULL_POINTER,
    FFT_ERR_INVALID_SIZE,
    FFT_ERR_WORK_TOO_SMALL,
    FFT_ERR_WORK_MISALIGNED,
} FFT_STATUS_E;

struct fft_plan_{{suffix}}_t {
    uint32_t n;
    uint32_t log2n;
    const complex_{{suffix}}_t *twiddle;
    uint32_t twiddle_stride; /* twiddle k is twiddle[k * twiddle_stride] */
};

/* For the transforms of n real samples */
struct rfft_plan_{{suffix}}_t {
    uint32_t n;
    const complex_{{suffix}}_t *twiddle;
    uint32_t twiddle_stride;
};
```

### Functions

| Function | Description |
| :--- | :--- |
| `fft_plan_size_{{suffix}}(n)` | Workspace bytes needed for an `n` point plan, or 0 if `n` is not a power of two of at least 2. |
| `FFT_PLAN_SIZE_{{SUFFIX}}(n)` | Compile time version of the above for sizing static buffers. Does not validate `n`. |
| `fft_plan_init_{{suffix}}(plan, n, work, work_bytes)` | Fills the twiddle table into the caller's `work` buffer and initialises `plan`. Returns an `FFT_STATUS_E`, and leaves `plan` unmodified on error. |
| `rfft_plan_size_{{suffix}}(n)`, `RFFT_PLAN_SIZE_{{SUFFIX}}(n)`, `rfft_plan_init_{{suffix}}(plan, n, work, work_bytes)` | The same for an `n` sample real plan, which needs the same workspace as an `n` point complex plan. |
| `fft_plan_init_table_{{suffix}}(plan, n)`, `rfft_plan_init_table_{{suffix}}(plan, n)` | *(`--fft-twiddle-table` only)* Initialise a plan from the generated table, for `n` up to `FFT_TABLE_MAX_N_{{SUFFIX}}`. Return `FFT_ERR_NULL_POINTER` or `FFT_ERR_INVALID_SIZE`, leaving `plan` unmodified. |
| `fft_{{suffix}}(plan, x)` | In place forward FFT of `plan->n` values. |
| `ifft_{{suffix}}(plan, x)` | In place inverse FFT of `plan->n` values, scaled by `1 / n`. |
| `rfft_{{suffix}}(plan, x)` | In place forward FFT of `plan->n` real values, returning bins 0 to `n / 2` packed into `x` (see below). About half the work of `fft_{{suffix}}()`. |
| `irfft_{{suffix}}(plan, x)` | In place inverse of `rfft_{{suffix}}()`, from the packed bins back to `plan->n` real values, scaled by `1 / n`. |
| `dft_{{suffix}}(data_in, data_out, n)` | Direct O(N²) DFT for any `n`, as a slow reference. The buffers must not overlap. Not generated with `--fft-table-only`. |

Nothing is allocated. The workspace holds `n / 2` twiddle factors and must
outlive the plan. A plan is read only after initialisation, so it can be shared
between threads transforming different buffers.

```c
static complex_float_t twiddle[1024 / 2];
static struct fft_plan_float_t plan;
fft_plan_init_float(&plan, 1024, twiddle, sizeof twiddle);
fft_float(&plan, x);
```

### Real input transform

`rfft_{{suffix}}()` packs the `n` real samples into `n / 2` complex values, runs an
`n / 2` point complex FFT and then one extra pass to separate the bins. The
other bins of a real signal are the complex conjugates (bin `n - k` of bin
`k`), so only bins 0 to `n / 2` are returned. Bins 0 and `n / 2` are both real,
so all of them fit in the `n` input values, in the same layout as CMSIS-DSP's
`arm_rfft_fast_f32()`:

| Index | Holds |
| :--- | :--- |
| `x[0]` | Bin 0 (real) |
| `x[1]` | Bin `n / 2` (real) |
| `x[2k]`, `x[2k + 1]` | Real and imaginary parts of bin `k`, for `0 < k < n / 2` |

This gives the same bins as `numpy.fft.rfft`, which returns them unpacked as
`n / 2 + 1` complex values. `irfft_{{suffix}}()` takes the same layout and, as
`numpy.fft.irfft` does, treats the imaginary parts of bins 0 and `n / 2` as 0.

```c
static float x[1024];
static struct rfft_plan_float_t plan;
rfft_plan_init_table_float(&plan, 1024); /* or rfft_plan_init_float() with a workspace */
rfft_float(&plan, x);
float dc = x[0], nyquist = x[1];
complex_float_t bin_k = {x[2 * k], x[2 * k + 1]};
```

### Example

`fft/main.c` transforms a pair of test tones with the float FFT and the inverse
FFT, and with the real FFT and its inverse, writing the results to CSV files in
`fft/results/`. Running `make` in
`fft/` builds and runs it, then `scripts/plot_results.py` compares the results
against numpy and plots them to `results/fft.png`.

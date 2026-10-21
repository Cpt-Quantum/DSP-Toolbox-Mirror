# DSP Toolbox C++ API Reference

This document describes the high-level C++ wrapper classes for the DSP-Toolbox C library. These classes provide a more idiomatic C++ interface, using `std::vector`, `std::unique_ptr`, and templates.

All wrapper classes are templates that support the following numerical types (`num_t`):

* `float`
* `double`
* `int32_t`
* `int64_t`

---

## Table of Contents

* [Headers and Sources](#headers-and-sources)
* [Waveform Generation (`waveform_x`)](#waveform-generation-waveform_x)
* [Streaming Generation, NCO / DDS (`nco`)](#streaming-generation-nco--dds-nco)
* [FIR Filters (`fir`)](#fir-filters-fir)
* [IIR Filters (Direct Form) (`iir_ba`)](#iir-filters-direct-form-iir_ba)
* [IIR Filters (SOS/Biquad) (`iir_sos`)](#iir-filters-sosbiquad-iir_sos)
* [Averaging (`average`, `power_average`, `moving_avg`)](#averaging-average-power_average-moving_avg)
* [Complex Math (`complex_t`, `polar_t`, `complex_*`)](#complex-math-complex_t-polar_t-complex_)
* [JSON Parsing (`json_filter_parse`)](#json-parsing-json_filter_parse)
* [Copying Filters (Multi-channel and Multi-threaded Use)](#copying-filters-multi-channel-and-multi-threaded-use)

---

## Headers and Sources

Each module has its own header, and `cpp_wrapper.h` includes all of them. Include only the modules you use, and build the matching sources from `cpp_wrapper.mk`:

| Header | Contents | Sources (`cpp_wrapper.mk`) |
|---|---|---|
| `cpp_wrapper_average.h` | `average`, `power_average`, `moving_avg` | `CPP_WRAPPER_AVERAGE_SRC` |
| `cpp_wrapper_complex.h` | `complex_t`, `polar_t`, the `complex_*` functions and operators | `CPP_WRAPPER_COMPLEX_SRC` |
| `cpp_wrapper_filter.h` | `json_filter_parse`, `fir`, `iir_ba`, `iir_sos` | `CPP_WRAPPER_FILTER_SRC` |
| `cpp_wrapper_waveform.h` | `waveform_t`, `waveform_x`, `nco` | `CPP_WRAPPER_WAVEFORM_SRC` |
| `cpp_wrapper.h` | All of the above | `CPP_WRAPPER_SRC` |

For example, a project using only the filters would set `CPP_WRAPPER_ROOT_DIR`, `include cpp_wrapper.mk`, then compile with `$(CPP_WRAPPER_FILTER_SRC)` and link with `$(CPP_WRAPPER_LDFLAGS)`. `cpp_data_io.h` uses `cpp_wrapper_waveform.h`, so it also needs `CPP_WRAPPER_WAVEFORM_SRC`.

---

## Waveform Generation (`waveform_x`)

Header: `cpp_wrapper_waveform.h`

A container and generator class for real-valued waveforms.

The times and wave properties are in `math_t`, the floating point type the waveforms are generated in (`wave_math_{{suffix}}_t` in C). It is `num_t` for `float` and `double`. For `int32_t` and `int64_t` it is chosen with `--int-waveform-precision` when the sources are generated (float and double by default), and only the samples in `x` are integers. `waveform_t<num_t>` creates a time series of `math_t`, `std::vector<math_t> t`, to pass to `gen_waveform()` and `gen_spectrum()`.

### Constructor

* `waveform_x(unsigned int wave_length)`: Creates a waveform container of the specified length.
* `waveform_x(unsigned int wave_length, std::vector<props_t> wave_properties)`: Creates a container and initializes it with a list of wave components.

### Methods

* `void add_wave_properties(math_t amplitude, math_t offset, math_t frequency, math_t phase, WAVEFORM_FUNC_E function_type, math_t duty_cycle = 0, math_t sweep_rate = 0, math_t delay = 0)`: Adds a new superposition component to the waveform definition, of any `WAVEFORM_FUNC_E` (sine, cosine, square, triangle, sawtooth, chirp, impulse or step). The last three are only used by the square (`duty_cycle`, where 0 gives 0.5), the chirp (`sweep_rate` in Hz per second) and the impulse and step (`delay` in seconds), see the C API reference.
* `void gen_waveform(std::vector<math_t> &t)`: Populates the internal `x` buffer based on the provided time vector `t`.
* `void gen_spectrum(std::vector<math_t> &t, std::size_t length, uint32_t f_s, uint32_t num_steps, AMPLITUDE_SPECTRUM_E profile, num_t base_amplitude, num_t noise_variance, uint32_t seed)`: Fills the waveform with `num_steps` sine waves shaped by `profile`, plus Gaussian noise with variance `noise_variance` that is reproducible from `seed`. See `generate_spectrum_waveform` in the [C API reference](../API_REFERENCE.md#spectrum-generation).
* `void scale(num_t factor)`: Multiplies all values in the waveform by `factor`.
* `void divide(num_t factor)`: Divides all values in the waveform by `factor`.
* `void mask(uint32_t bit_width)`: Saturates values to the range of a signed integer of `bit_width`.

---

## Streaming Generation, NCO / DDS (`nco`)

Header: `cpp_wrapper_waveform.h`

A numerically controlled oscillator, the digital part of a direct digital synthesiser (DDS), generating a periodic waveform a block or a sample at a time and carrying on from where the last call left off. See [Streaming Generation (NCO / DDS)](../API_REFERENCE.md#streaming-generation-nco--dds) in the C API reference for the supported functions, accuracy and generation options. `math_t` is the same as for `waveform_x`, and `complex_t` is the C complex struct for `num_t` (e.g. `complex_float_t`).

### Constructors

* `nco(props_t props, uint32_t sample_rate)`: From a `wave_properties` struct, generating the same wave as `waveform_x` with the same properties.
* `nco(math_t amplitude, math_t offset, math_t frequency, math_t phase, WAVEFORM_FUNC_E function_type, uint32_t sample_rate, math_t duty_cycle = 0, math_t sweep_rate = 0)`: The same, from the individual properties.

Both throw `std::invalid_argument` for a sample rate of 0, or for `FUNCTION_IMPULSE` or `FUNCTION_STEP`, which aren't periodic. A copy of an `nco` carries on independently from the same point.

### Methods

* `void set_frequency(math_t frequency)`, `void set_sweep(math_t sweep_rate)`, `void set_phase(math_t phase)`: Change the oscillator from the next sample, see `nco_set_*()` in the C API reference. Frequency changes are phase continuous.
* `num_t sample()`, `complex_t sample_complex()`: The next sample.
* `void generate(std::vector<num_t> &x)`, `void generate_complex(std::vector<complex_t> &x)`: Fill `x` with the next `x.size()` samples.
* `void add(std::vector<num_t> &x)`, `void add_complex(std::vector<complex_t> &x)`: Add the next `x.size()` samples to `x`.
* `generate`, `generate_complex`, `add` and `add_complex` also have pointer overloads taking `(num_t *x, uint32_t length)`, for working at an offset within a larger buffer.

The complex output is the wave plus `j` times the same wave a quarter of a period earlier, so `FUNCTION_COS` gives `amplitude * e^(j 2 pi (frequency * t + phase))`.

---

## FIR Filters (`fir`)

Header: `cpp_wrapper_filter.h`

A wrapper for Finite Impulse Response filtering.

### Constructors

* `fir(num_t *b, num_t *in_buff, uint16_t taps)`: Initializes with raw coefficient and history buffers. `in_buff` must be zeroed and hold `FILTER_STATE_SIZE(taps)` elements.
* `fir(num_t *b, num_t *in_buff, uint16_t taps, uint8_t frac_bits)`: Integer types only, the same as above for Q format coefficients with `frac_bits` fractional bits. See [Integer filters](../API_REFERENCE.md#integer-filters-fixed-point).
* `fir(cJSON *node)`: Initializes automatically from a cJSON object (e.g., loaded from a file).

### Methods

* `void fir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length)`: Processes input data through the filter.
* `void fir_filter(num_t *data_in, num_t *data_out, uint32_t length)`: Pointer overload of the above, for filtering at an offset within a larger buffer without copying.
* `uint32_t fir_decimate(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length, uint8_t decimation_rate)`: Filters and reduces sample rate, returning the number of output samples written. `length` does not need to be a multiple of the rate; `data_out` must hold at least `ceil(length / decimation_rate)` samples.
* `uint32_t fir_decimate(num_t *data_in, num_t *data_out, uint32_t length, uint8_t decimation_rate)`: Pointer overload of the above, for filtering at an offset within a larger buffer without copying.
* `uint8_t get_frac_bits() const`: Integer types only, the Q format fractional bits of the coefficients.

---

## IIR Filters (Direct Form) (`iir_ba`)

Header: `cpp_wrapper_filter.h`

A wrapper for Direct Form I (ba) Infinite Impulse Response filters.

### Constructors

* `iir_ba(num_t *a, num_t *b, num_t *in_buff, num_t *out_buff, uint16_t taps)`: Initializes with raw buffers. `in_buff` and `out_buff` must be zeroed and hold `FILTER_STATE_SIZE(taps)` elements each.
* `iir_ba(num_t *a, num_t *b, num_t *in_buff, num_t *out_buff, uint16_t taps, uint8_t frac_bits)`: Integer types only, for Q format coefficients with `frac_bits` fractional bits. `a[0]` must be `1 << frac_bits`.
* `iir_ba(cJSON *node)`: Initializes automatically from a cJSON object.

### Methods

* `void iir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length)`: Processes input data.
* `void iir_filter(num_t *data_in, num_t *data_out, uint32_t length)`: Pointer overload of the above, for filtering at an offset within a larger buffer without copying.
* `uint8_t get_frac_bits() const`: Integer types only, the Q format fractional bits of the coefficients.

---

## IIR Filters (SOS/Biquad) (`iir_sos`)

Header: `cpp_wrapper_filter.h`

A wrapper for Second Order Sections (SOS) IIR filters, which are more numerically stable than Direct Form for higher-order filters.

### Constructors

* `iir_sos(biquad_t *sections, uint16_t n_sections, num_t gain)`: Initializes with raw SOS sections.
* `iir_sos(biquad_t *sections, uint16_t n_sections, num_t gain, uint8_t frac_bits)`: Integer types only, for Q format section coefficients and gain with `frac_bits` fractional bits. Each section's `a[0]` must be `1 << frac_bits`.
* `iir_sos(cJSON *node)`: Initializes automatically from a cJSON object.

### Methods

* `void iir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length)`: Processes input data through all sections.
* `void iir_filter(num_t *data_in, num_t *data_out, uint32_t length)`: Pointer overload of the above, for filtering at an offset within a larger buffer without copying.
* `uint8_t get_frac_bits() const`: Integer types only, the Q format fractional bits of the coefficients.

---

## Averaging (`average`, `power_average`, `moving_avg`)

Header: `cpp_wrapper_average.h`

Wrappers for the averaging functions in the [C API reference](../API_REFERENCE.md#averaging). The integer types sum in `int64_t` and round each average to the nearest value.

### Functions

* `num_t average(const std::vector<num_t> &data_in)`: Mean of the samples, 0 if there are none.
* `num_t average(const num_t *data_in, uint32_t length)`: Pointer overload of the above, for averaging part of a larger buffer without copying.
* `power_type power_average(const std::vector<num_t> &data_in)`: Mean of the squares of the samples, 0 if there are none. `power_type` is `avg_traits<num_t>::power_type`: `int64_t` for `int32_t`, because the squares don't fit in `int32_t`, and `num_t` otherwise.
* `power_type power_average(const num_t *data_in, uint32_t length)`: Pointer overload of the above.

### Moving Average (`moving_avg`)

#### Constructors

* `moving_avg(uint32_t length)`: Creates an empty window of `length` samples, owned by the object. Throws `std::invalid_argument` if `length` is 0.
* `moving_avg(num_t *buff, uint32_t length)`: Uses the caller's buffer of `length` samples as the window, which must outlive the object.

#### Methods

* `num_t moving_average(num_t new_samp)`: Adds a sample to the window and returns the new average. Until the window is full, this is the average of the samples received so far.
* `void moving_average(const std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length)`: Averages each sample of `data_in` into `data_out`, which may be the same vector.
* `void moving_average(const num_t *data_in, num_t *data_out, uint32_t length)`: Pointer overload of the above, for averaging at an offset within a larger buffer without copying.
* `void reset()`: Empties the window.
* `uint32_t get_length() const`: The window length.
* `uint32_t get_count() const`: The number of samples in the window, up to the window length.

Copies of a `moving_avg` have the same window length, with their own empty window, so one reference can be copied for each channel. Moving one keeps its window and state. See `examples/average`, which averages several channels in float and int32.

---

## Complex Math (`complex_t`, `polar_t`, `complex_*`)

Header: `cpp_wrapper_complex.h`

Wrappers for the complex functions in the [C API reference](../API_REFERENCE.md#complex-math), working on the C structs directly. `complex_t<num_t>` and `polar_t<num_t>` are the C structs for `num_t`, e.g. `complex_t<float>` is `complex_float_t`. The functions are templates on the struct type, so the type is deduced from the arguments, and they only accept the complex structs (and the polar structs for `complex_from_polar`).

The `int32_t` and `int64_t` types are fixed point, rounded and saturated, see [Integer complex numbers](../API_REFERENCE.md#integer-complex-numbers-fixed-point). Their `complex_multiply` and `complex_divide` take the number of fractional bits of the format as a trailing `uint32_t frac_bits` (e.g. 31 for Q31), and the float and double versions don't. The phase is in `math_type`, `complex_traits<num_t>::math_type`: `num_t` for float and double, float for `int32_t` and double for `int64_t`.

### Functions

`C` is a complex struct, such as `complex_t<float>`, and `P` the polar struct of the same type.

* `C complex_add(C x, C y)`, `C complex_subtract(C x, C y)`
* `C complex_multiply(C x, C y)`, `C complex_divide(C x, C y)`: float and double only.
* `C complex_multiply(C x, C y, uint32_t frac_bits)`, `C complex_divide(C x, C y, uint32_t frac_bits)`: integer types only.
* `C complex_conjugate(C x)`
* `num_t complex_magnitude(C x)`
* `math_type complex_phase(C x)`: `atan2(imag, real)` in radians, from -pi to pi.
* `P complex_to_polar(C x)`, `C complex_from_polar(P p)`

### Operators

* `x + y`, `x - y` for all the complex structs.
* `x * y`, `x / y` for float and double only, as the integer types need `frac_bits`.

### Vector and pointer versions

Each function above also takes vectors, applying it to each element, e.g. `void complex_multiply(const std::vector<C> &x, const std::vector<C> &y, std::vector<C> &out)`, with `frac_bits` last for the integer types. `out` is resized to the length of `x`, and may be the same vector as `x` or `y`, so `complex_multiply(x, osc, x)` works in place. Throws `std::invalid_argument` if `x` and `y` are different lengths. `complex_magnitude` writes a `std::vector<num_t>`, `complex_phase` a `std::vector<math_type>`, `complex_to_polar` a `std::vector<P>`, and `complex_from_polar` takes a `std::vector<P>`.

The pointer overloads take a length instead, e.g. `void complex_add(const C *x, const C *y, C *out, uint32_t length)`, for working at an offset within a larger buffer without copying. `out` may be the same buffer as `x` or `y`.

See `examples/complex`, which mixes a complex tone down in frequency in float and Q31.

---

## JSON Parsing (`json_filter_parse`)

Header: `cpp_wrapper_filter.h`

A helper class for managing cJSON file lifecycles and node extraction.

### Constructor

* `json_filter_parse(const std::string &filename)`: Opens and parses the specified JSON file. Throws `std::runtime_error` on failure.

### Methods

* `cJSON *get_root()`: Returns the root node of the parsed JSON.
* `cJSON *get_node(const std::string &node_string)`: Retrieves a specific node by name.

---

## Copying Filters (Multi-channel and Multi-threaded Use)

To filter many channels with the same design, load one reference filter and copy it once per channel, e.g. `std::vector<fir<float>> channel_filters(n_channels, ref_filt);`.

* Copies **share** the reference filter's coefficients, which are only ever read. The reference filter must outlive all of its copies.
* Each copy has its **own** filter state:
  * `fir` and `iir_ba` copies start with zeroed history buffers (and, for `fir`, a reset decimation phase).
  * `iir_sos` copies take a copy of the reference filter's current section state, so copy from a filter that hasn't been run yet to start from zero.

Because the only shared data is read-only, different copies can be run on different threads at the same time without locking. A single filter object must not be used from more than one thread at once. See `examples/multithread` for an example that splits channels across threads.

---

## Implementation Details

The C++ wrappers utilize **Traits** to map template types to the correct C function calls (e.g., `fir_traits<float>` calls `fir_filter_float`). Each header follows the same layout: the C headers, the standard headers, then a `<module>_traits<num_t>` struct with float, double, int32_t and int64_t specializations in that order, then the public classes and functions. All memory management for C-allocated arrays is handled via custom deleters in `std::unique_ptr` or through `std::vector` for high-level data.

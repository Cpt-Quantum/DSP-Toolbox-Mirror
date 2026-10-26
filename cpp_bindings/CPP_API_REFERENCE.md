# DSP Toolbox C++ API Reference

This document describes the high-level C++ wrapper classes for the DSP-Toolbox C library. These classes provide a more idiomatic C++ interface, using `std::vector`, `std::unique_ptr`, and templates.

All wrapper classes are templates that support the following numerical types (`num_t`):

* `float`
* `double`
* `int32_t`
* `int64_t`

The FFT classes are only generated for `float` and `double`.

---

## Table of Contents

* [Headers and Building](#headers-and-building)
* [Waveform Generation (`waveform_x`)](#waveform-generation-waveform_x)
* [Streaming Generation, NCO / DDS (`nco`)](#streaming-generation-nco--dds-nco)
* [FIR Filters (`fir`)](#fir-filters-fir)
* [IIR Filters (Direct Form) (`iir_ba`)](#iir-filters-direct-form-iir_ba)
* [IIR Filters (SOS/Biquad) (`iir_sos`)](#iir-filters-sosbiquad-iir_sos)
* [Averaging and Statistics (`average`, `power_average`, `rms`, `variance`, `moving_avg`, ...)](#averaging-and-statistics-average-power_average-rms-variance-moving_avg-)
* [Complex Math (`complex_t`, `polar_t`, `complex_*`)](#complex-math-complex_t-polar_t-complex_)
* [FFT (`fft`, `rfft`, `dft`)](#fft-fft-rfft-dft)
* [JSON Parsing (`json_filter_parse`)](#json-parsing-json_filter_parse)
* [Copying Filters (Multi-channel and Multi-threaded Use)](#copying-filters-multi-channel-and-multi-threaded-use)

---

## Headers and Building

Each module has its own header, and `cpp_wrapper.h` includes all of them. Include only the modules you use:

| Header | Contents |
|---|---|
| `cpp_wrapper_average.h` | `average`, `power_average`, `moving_avg` |
| `cpp_wrapper_complex.h` | `complex_t`, `polar_t`, the `complex_*` functions and operators |
| `cpp_wrapper_fft.h` | `fft`, `rfft`, `dft` |
| `cpp_wrapper_filter.h` | `json_filter_parse`, `fir`, `iir_ba`, `iir_sos` |
| `cpp_wrapper_waveform.h` | `waveform_t`, `waveform_x`, `nco` |
| `cpp_wrapper.h` | All of the above |

The C code is compiled as C by the top level `Makefile` into `build/libdsptoolbox.a`, which every module links against. To use it from your own makefile, set `CPP_WRAPPER_ROOT_DIR` to the root of the repository and `include cpp_wrapper.mk`, then make your target depend on `$(CPP_WRAPPER_LIB)` and link with `$(CPP_WRAPPER_LDFLAGS)` after your sources:

```make
CPP_WRAPPER_ROOT_DIR = path/to/DSP-Toolbox
include $(CPP_WRAPPER_ROOT_DIR)/cpp_bindings/cpp_wrapper.mk

my_app: $(CPP_WRAPPER_LIB)
	$(CXX) $(CXXFLAGS) my_app.cpp -o my_app $(CPP_WRAPPER_LDFLAGS)
```

Depending on `$(CPP_WRAPPER_LIB)` runs the top level build first, which generates the C sources and rebuilds whatever has changed. Generation options are set with `GEN_ARGS` on the make command line, see the [README](../README.md#getting-started).

To link the shared library `build/libdsptoolbox.so` instead, use `$(CPP_WRAPPER_SHARED_LIB)` and `$(CPP_WRAPPER_SHARED_LDFLAGS)`. The program is linked with an rpath to the `build/` directory, so it finds the library without setting `LD_LIBRARY_PATH`:

```make
my_app: $(CPP_WRAPPER_SHARED_LIB)
	$(CXX) $(CXXFLAGS) my_app.cpp -o my_app $(CPP_WRAPPER_SHARED_LDFLAGS)
```

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

## Averaging and Statistics (`average`, `power_average`, `rms`, `variance`, `moving_avg`, ...)

Header: `cpp_wrapper_average.h`

Wrappers for the averaging and statistics functions in the [C API reference](../API_REFERENCE.md#averaging). The integer types sum in `int64_t` and round each result to the nearest value.

### Functions

* `num_t average(const std::vector<num_t> &data_in)`: Mean of the samples, 0 if there are none.
* `num_t average(const num_t *data_in, uint32_t length)`: Pointer overload of the above, for averaging part of a larger buffer without copying.
* `power_type power_average(const std::vector<num_t> &data_in)`: Mean of the squares of the samples, 0 if there are none. `power_type` is `avg_traits<num_t>::power_type`: `int64_t` for `int32_t`, because the squares don't fit in `int32_t`, and `num_t` otherwise.
* `power_type power_average(const num_t *data_in, uint32_t length)`: Pointer overload of the above.
* `num_t rms(const std::vector<num_t> &data_in)`: Root mean square of the samples, 0 if there are none. Rounded to the nearest value for the integer types. For float and double, generating with `--no-stdlib` leaves it out (it needs the maths library), and using it is a compile error.
* `power_type variance(const std::vector<num_t> &data_in)`: Population variance (as `numpy.var`), 0 if there are none. `int64_t` for `int32_t`, rounded to the nearest value for the integer types.
* `num_t std_dev(const std::vector<num_t> &data_in)`: Population standard deviation, 0 if there are none. Rounded for the integer types, and left out like `rms` with `--no-stdlib`.
* `num_t minimum(const std::vector<num_t> &data_in, uint32_t *index = nullptr)`, `num_t maximum(...)`: Smallest or largest sample, 0 if there are none. `index`, if given, is set to the index of the first occurrence. NaNs are skipped unless every sample is NaN.
* `uint32_t zero_crossings(const std::vector<num_t> &data_in)`: Number of sign changes between consecutive samples, with 0 counted as positive.

Each of these also has a pointer overload taking `(const num_t *data_in, uint32_t length)` before any other arguments, for part of a larger buffer without copying.

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

## FFT (`fft`, `rfft`, `dft`)

Header: `cpp_wrapper_fft.h`

Wrappers for the radix-2 transforms in the [C API reference](../API_REFERENCE.md#fft), for `float` and `double` only. The transform size `n` is a power of two of at least 2. The forward transforms are unscaled and the inverses are scaled by `1 / n`, matching `numpy.fft`, with the output in natural order. `complex_t` is the C complex struct for `num_t`, the same type as `complex_t<num_t>` in `cpp_wrapper_complex.h`, and is also available as `fft<num_t>::complex_type`.

The plan initialisations available depend on the [generation options](../API_REFERENCE.md#generation-options-twiddles):

| Generated with | `fft(n)` / `rfft(n)` | Caller's workspace | `from_table(n)` | `dft` |
|---|---|---|---|---|
| *(default)* | Owned workspace | Yes | No | Yes |
| `--fft-twiddle-table MAX_N` | Owned workspace | Yes | Yes | Yes |
| `--fft-twiddle-table MAX_N --fft-table-only` | The generated table | No | Yes | No |

Using a member that wasn't generated is a compile error, starting with a `static_assert` naming the option it needs. `table_max_n` is 0 without `--fft-twiddle-table`.

### Complex FFT (`fft`)

#### Constructors

* `fft(uint32_t n)`: Plans `n` point transforms, with the twiddle workspace owned by the object (from the table with `--fft-table-only`).
* `fft(uint32_t n, void *work, size_t work_bytes)`: Uses the caller's workspace, which must outlive the object and its copies. `static size_t workspace_size(uint32_t n)` gives the bytes needed, 0 if `n` is invalid.
* `static fft from_table(uint32_t n)`: *(`--fft-twiddle-table` only)* Plans from the generated `const` table, with no workspace, for `n` up to `fft<num_t>::table_max_n`.

All of them throw `std::invalid_argument` with the reason for any `FFT_STATUS_E` error.

#### Methods

* `void forward(std::vector<complex_t> &x) const`, `void inverse(std::vector<complex_t> &x) const`: In place transforms. Throw `std::invalid_argument` if `x` doesn't hold `size()` values.
* `void forward(const std::vector<complex_t> &x, std::vector<complex_t> &out) const`, and the same for `inverse`: Out of place, with `out` resized to `size()`. `out` may be the same vector as `x`.
* `void forward(complex_t *x) const`, `void inverse(complex_t *x) const`: Pointer overloads, for transforming at an offset within a larger buffer. `x` must hold `size()` values.
* `uint32_t size() const`: The transform size.
* `const plan_t &get_plan() const`: The C plan (`fft_plan_{{suffix}}_t`), for calling the C functions directly.

### Real FFT (`rfft`)

The transforms of `n` real samples, about half the work of `fft` for the same `n`. The constructors, `workspace_size`, `from_table`, `table_max_n`, `size()` and `get_plan()` are the same as for `fft`, with `get_plan()` giving an `rfft_plan_{{suffix}}_t`.

#### Methods

* `void forward(std::vector<num_t> &x) const`, `void inverse(std::vector<num_t> &x) const`: In place, in the packed layout of the [real input transform](../API_REFERENCE.md#real-input-transform) (`x[0]` bin 0, `x[1]` bin `n / 2`, then the real and imaginary parts of bins 1 to `n / 2 - 1`). Throw `std::invalid_argument` if `x` doesn't hold `size()` values.
* `void forward(const std::vector<num_t> &x, std::vector<complex_t> &bins) const`: Bins 0 to `n / 2`, unpacked into `bins`, which is resized to `n / 2 + 1`. The same as `numpy.fft.rfft`.
* `void inverse(const std::vector<complex_t> &bins, std::vector<num_t> &x) const`: The `n` real samples from the `n / 2 + 1` unpacked bins, with `x` resized to `n`. The imaginary parts of bins 0 and `n / 2` are taken as 0, as `numpy.fft.irfft` does. Throws `std::invalid_argument` if `bins` doesn't hold `n / 2 + 1` values.
* `void forward(num_t *x) const`, `void inverse(num_t *x) const`: Pointer overloads of the packed transforms.
* `void forward(const num_t *x, complex_t *bins) const`, `void inverse(const complex_t *bins, num_t *x) const`: Pointer overloads of the unpacked transforms. `x` holds `n` values and `bins` `n / 2 + 1`, and they must not overlap.

### DFT (`dft`)

* `void dft(const std::vector<C> &x, std::vector<C> &out)`: Direct O(N²) DFT of any length, as a slow reference. `C` is `complex_t<float>` or `complex_t<double>`, deduced from the arguments. `out` is resized to the length of `x`, and must be a different vector (throws `std::invalid_argument` if not).
* `void dft(const C *data_in, C *data_out, uint32_t length)`: Pointer overload, the buffers must not overlap.

### Copies and threads

The twiddles are read only after the plan is initialised, so copies of an `fft` or `rfft` share them, including an owned workspace, which lives until the last copy is destroyed. The transform methods are `const`, so one object can be used by several threads at once, as long as each transforms a different buffer.

```cpp
fft<float> plan(1024);
std::vector<complex_t<float>> x(1024);
plan.forward(x);
plan.inverse(x);

rfft<float> rplan(1024);
std::vector<float> samples(1024);
std::vector<complex_t<float>> bins;
rplan.forward(samples, bins); /* bins 0 to 512 */
```

See `examples/fft`, which takes the real FFT of a generated signal with `rfft<float>` and plots its spectrum against `numpy.fft.rfft`. `examples/multithread` also benchmarks `rfft` across several threads, sharing one plan between the channels.

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

The C++ wrappers utilize **Traits** to map template types to the correct C function calls (e.g., `fir_traits<float>` calls `fir_filter_float`). The traits are generated with the C sources, from `templates/cpp_traits_<module>.h.j2` into `cpp_bindings/cpp_traits_<module>.h`, with a specialization for each type in the generator's type configuration (float and double only for `fft_traits`). They include the module's C headers, and reflect the generation options, e.g. `fft_traits::workspace_plans` and `fft_traits::table_max_n`. Each `cpp_wrapper_<module>.h` includes its traits header and holds the hand-written public classes and functions, so supporting a new C function means adding it to the traits template and calling it from the class. All memory management for C-allocated arrays is handled via custom deleters in `std::unique_ptr` or through `std::vector` for high-level data.

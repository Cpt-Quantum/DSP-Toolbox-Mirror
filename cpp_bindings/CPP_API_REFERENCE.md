# DSP Toolbox C++ API Reference

This document describes the high-level C++ wrapper classes available in `cpp_wrapper.h`. These classes provide a more idiomatic C++ interface for the underlying DSP-Toolbox C library, using `std::vector`, `std::unique_ptr`, and templates.

All wrapper classes are templates that support the following numerical types (`num_t`):

* `float`
* `double`
* `int32_t`
* `int64_t`

---

## Table of Contents

* [Waveform Generation (`waveform_x`)](#waveform-generation-waveform_x)
* [FIR Filters (`fir`)](#fir-filters-fir)
* [IIR Filters (Direct Form) (`iir_ba`)](#iir-filters-direct-form-iir_ba)
* [IIR Filters (SOS/Biquad) (`iir_sos`)](#iir-filters-sosbiquad-iir_sos)
* [JSON Parsing (`json_filter_parse`)](#json-parsing-json_filter_parse)

---

## Waveform Generation (`waveform_x`)

A container and generator class for real-valued waveforms.

### Constructor

* `waveform_x(unsigned int wave_length)`: Creates a waveform container of the specified length.
* `waveform_x(unsigned int wave_length, std::vector<props_t> wave_properties)`: Creates a container and initializes it with a list of wave components.

### Methods

* `void add_wave_properties(num_t amplitude, num_t offset, num_t frequency, num_t phase, WAVEFORM_FUNC_E function_type)`: Adds a new superposition component (Sine/Cos) to the waveform definition.
* `void gen_waveform(std::vector<num_t> &t)`: Populates the internal `x` buffer based on the provided time vector `t`.
* `void gen_spectrum(...)`: Generates a complex test pattern based on a frequency profile.
* `void scale(num_t factor)`: Multiplies all values in the waveform by `factor`.
* `void divide(num_t factor)`: Divides all values in the waveform by `factor`.
* `void mask(uint32_t bit_width)`: Saturates values to the range of a signed integer of `bit_width`.

---

## FIR Filters (`fir`)

A wrapper for Finite Impulse Response filtering.

### Constructors

* `fir(num_t *b, num_t *in_buff, uint16_t taps)`: Initializes with raw coefficient and delay line buffers.
* `fir(cJSON *node)`: Initializes automatically from a cJSON object (e.g., loaded from a file).

### Methods

* `void fir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length)`: Processes input data through the filter.
* `void fir_decimate(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length, uint8_t decimation_rate)`: Filters and reduces sample rate.

---

## IIR Filters (Direct Form) (`iir_ba`)

A wrapper for Direct Form I (ba) Infinite Impulse Response filters.

### Constructors

* `iir_ba(num_t *a, num_t *b, num_t *in_buff, num_t *out_buff, uint16_t taps)`: Initializes with raw buffers.
* `iir_ba(cJSON *node)`: Initializes automatically from a cJSON object.

### Methods

* `void iir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length)`: Processes input data.

---

## IIR Filters (SOS/Biquad) (`iir_sos`)

A wrapper for Second Order Sections (SOS) IIR filters, which are more numerically stable than Direct Form for higher-order filters.

### Constructors

* `iir_sos(biquad_t *sections, uint16_t n_sections, num_t gain)`: Initializes with raw SOS sections.
* `iir_sos(cJSON *node)`: Initializes automatically from a cJSON object.

### Methods

* `void iir_filter(std::vector<num_t> &data_in, std::vector<num_t> &data_out, uint32_t length)`: Processes input data through all sections.

---

## JSON Parsing (`json_filter_parse`)

A helper class for managing cJSON file lifecycles and node extraction.

### Constructor

* `json_filter_parse(const std::string &filename)`: Opens and parses the specified JSON file. Throws `std::runtime_error` on failure.

### Methods

* `cJSON *get_root()`: Returns the root node of the parsed JSON.
* `cJSON *get_node(const std::string &node_string)`: Retrieves a specific node by name.

---

## Implementation Details

The C++ wrappers utilize **Traits** to map template types to the correct C function calls (e.g., `fir_traits<float>` calls `fir_filter_float`). All memory management for C-allocated arrays is handled via custom deleters in `std::unique_ptr` or through `std::vector` for high-level data.

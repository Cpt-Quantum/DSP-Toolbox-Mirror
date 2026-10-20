# DSP Toolbox

![Filter benchmark example](filter_benchmark.png)

This repository contains a collection of tools that aid in the development and
design of signal processing algorithms. There are filter implementations in C
that allow FIR/IIR filters to be calculated against floats, doubles and
integers. There are also waveform generation tools to make generating test data
simpler.

There are C++ wrappers to make interaction in a C++ environment smoother, this
allows wrapping of different types more clean. It potentially allows for better
abstractions to be made available.

Although there is a python bindings folder, these are still a work in progress.
Do not use them until more work has been done to fix these (pull requests
welcome).

# Getting started

In order to use this library, you'll need to run the generate script for the C
files. This can be done from the root of the project as:

```shell
python scripts/generate_sources.py
```

For targets without a C standard library (e.g. MCUs), or to use your own copy
functions in the filters, see the options in
`python scripts/generate_sources.py --help` and the
[C API documentation](API_REFERENCE.md#generation-options-copy-functions).

# API documentation

- [C functionality](API_REFERENCE.md)
- [C++ bindings](/cpp_bindings/CPP_API_REFERENCE.md)

# C++ examples

Examples using the C++ bindings are in `cpp_bindings/examples/`. Generate the C
sources first (see above), then build each example with `make` from inside its
folder:

| Example | Build | Description |
| :--- | :--- | :--- |
| `decimation` | `make decimation` | Benchmarks FIR decimation of 288 channels on a single thread, comparing two memory layouts: one reused chunk buffer (path A) and a buffer per channel (path B). `make verify` checks that both paths give identical output. |
| `multithread` | `make multithread` | Benchmarks FIR, IIR (SOS) and IIR (direct form) filters designed to the same lowpass specification, comparing 1 thread against `N_THREADS` (default 4) with the channels split evenly across the threads. Checks that the multi-threaded output is identical. |
| `spectrum_test` | `make spectrum` | Generates a test spectrum and prints its power. |
| `waveform_gen` | `make spectrum` | Generates multi-channel waveforms to a CSV file, plotted with `plot_waveform_gen.py`. |

The `decimation` and `multithread` examples also have a `run_all` target that
runs the benchmark and then plots the results. They generate a large input
data file (around 230 MB) in their `results/` folder on the first run, and
reuse it afterwards unless built with `REGEN_INPUT_FILE=true`. The filter
designs are stored in JSON files next to each example, and can be regenerated
with the `filter_design.py` script in that folder.

# cJSON

This project uses the cJSON library for parsing filters exported in JSON format.
The license for this project doesn't apply to the files in cjson/, the original
license from that project is included in that folder to avoid confusion.

# jinja2 templates

To make maintenance of the C implementations more easy, a lot of code is
generated from jinja2 templates. This allows support for multiple C types
without having to manually copy-paste similarities between different C functions
that just need to handle different types.

# Octave

There are a variety of Octave/Matlab scripts dotted around for visualising data
and in some cases generating filter coefficients and structures. These aren't
strictly necessary to run the code examples, but it is a nice to have for DSP
development in general.

# Python packages

This project uses python scripts for generating filter coefficients/structures,
as well as interpreting results. As a result there are a few dependencies that
you will need:

- jinja2
- matplotlib
- numpy
- pandas
- scipy

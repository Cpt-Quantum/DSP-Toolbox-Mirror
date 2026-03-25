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
as well as interpeting results. As a result there are a few dependencies that
you will need:

- jinja2
- matplotlib
- numpy
- pandas
- scipy

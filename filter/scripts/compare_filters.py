import json
import sys

import numpy as np
import pandas as pd
import scipy.signal as signal

RESULTS_DIR = "results"
DECIMATION_RATE = 4

# Output file prefix and the filter design it was run with
FILTERS = [
    ("iir_simple", "iir_base"),
    ("iir_sos", "iir_sos"),
    ("fir", "fir"),
    ("fir_decimation", "fir"),
]


def coefficients(design, key):
    """The design's coefficients as floats, undoing the Q format of the
    integer designs"""
    scale = 2.0 ** design.get("frac_bits", 0)
    return np.array(design[key], dtype=float) / scale


def reference(design, x, decimate):
    """Double precision SciPy output of the filter design"""
    if "sections" in design:
        y = signal.sosfilt(coefficients(design, "sections"), x)
        y *= float(design["gain"]) / 2.0 ** design.get("frac_bits", 0)
    elif "a" in design:
        y = signal.lfilter(coefficients(design, "b"), coefficients(design, "a"), x)
    else:
        y = signal.lfilter(coefficients(design, "b"), 1.0, x)
    return y[::DECIMATION_RATE] if decimate else y


def snr_db(actual, expected):
    noise = np.mean((actual - expected) ** 2)
    return 10 * np.log10(np.mean(expected**2) / noise) if noise > 0 else np.inf


def run(results_dir):
    designs = json.load(open(f"{results_dir}/filters.json"))
    x = pd.read_csv(f"{results_dir}/input_data.csv")["Amplitude"].values

    # Both are measured against their own coefficients, so this is the error of
    # the filter arithmetic. The int32 designs also differ from the float
    # designs by the coefficient quantisation, shown in the last column
    print("SNR against a double precision SciPy reference (dB)")
    print(f"{'Filter':16s}{'float':>10s}{'int32':>10s}{'Quantised design':>20s}")
    for output, name in FILTERS:
        decimate = output == "fir_decimation"
        y_float = pd.read_csv(f"{results_dir}/{output}_data.csv")["Amplitude"].values
        y_int = pd.read_csv(f"{results_dir}/{output}_int32_data.csv")["Amplitude"].values
        ref_float = reference(designs[name], x, decimate)
        ref_int = reference(designs[f"{name}_int32"], x, decimate)
        print(
            f"{output:16s}{snr_db(y_float, ref_float):10.1f}{snr_db(y_int, ref_int):10.1f}"
            f"{snr_db(ref_int, ref_float):20.1f}"
        )


if __name__ == "__main__":
    results_dir = sys.argv[1] if len(sys.argv) > 1 else RESULTS_DIR
    run(results_dir)

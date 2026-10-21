import sys

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy import signal

matplotlib.use("Agg")  # Headless mode

RESULTS_DIR = "results"
# Edges closer than this (in periods) to a sample are skipped when comparing
# the square and sawtooth waves, as rounding of t can put the sample either side
EDGE_TOLERANCE = 1e-4


def near_edge(cycles, edges):
    """True for samples within EDGE_TOLERANCE periods of any of the edges,
    given as fractions of a period"""
    fraction = np.mod(cycles, 1)
    distance = [np.abs(((fraction - e) + 0.5) % 1 - 0.5) for e in edges]
    return np.min(distance, axis=0) < EDGE_TOLERANCE


def first_at_or_after(t, delay):
    """Matches FUNCTION_IMPULSE, at the first sample at or after delay"""
    impulse = np.zeros_like(t)
    impulse[np.argmax(t >= delay)] = 1
    return impulse


def references(t, t32):
    """scipy/numpy references for each wave in main.c. The impulse and step
    compare the float times in the same way as the C code"""
    return {
        # (reference, samples to skip)
        "Square": (signal.square(2 * np.pi * 7 * t, duty=0.3), near_edge(7 * t, [0, 0.3])),
        "Triangle": (signal.sawtooth(2 * np.pi * 7 * t, width=0.5), None),
        "Sawtooth": (signal.sawtooth(2 * np.pi * (7 * t + 0.25)), near_edge(7 * t + 0.25, [0])),
        "Chirp": (signal.chirp(t, f0=10, t1=1, f1=100, method="linear"), None),
        "Impulse": (first_at_or_after(t32, np.float32(0.25)), None),
        "Step": ((t32 >= np.float32(0.5)).astype(float), None),
        "Composite": (
            (t32 >= np.float32(0.2))
            + 2 * first_at_or_after(t32, np.float32(0.6))
            + 0.2 * signal.square(2 * np.pi * 23 * t)
            + 0.5 * np.sin(2 * np.pi * 3 * t),
            near_edge(23 * t, [0, 0.5]),
        ),
    }


def run(results_dir):
    df = pd.read_csv(f"{results_dir}/waveform_data.csv")
    t = df["Time"].values
    t32 = t.astype(np.float32)

    # The C waves are generated in single precision, so expect errors around 1e-6
    refs = references(t, t32)
    for name, (ref, skip) in refs.items():
        keep = np.ones_like(t, dtype=bool) if skip is None else ~skip
        error = np.max(np.abs(df[name].values[keep] - ref[keep]))
        skipped = f" ({np.sum(~keep)} sample(s) on an edge skipped)" if skip is not None else ""
        print(f"{name + ':':11s} max error against scipy {error:.3g}{skipped}")

    fig, axes = plt.subplots(len(refs), 1, figsize=(10, 2 * len(refs)), sharex=True)
    for ax, (name, (ref, _)) in zip(axes, refs.items()):
        ax.plot(t, ref, label="scipy", alpha=0.4, linewidth=3)
        ax.plot(t, df[name], label="C")
        ax.set_title(name)
    axes[0].legend(loc="upper right")
    axes[-1].set_xlabel("Time (s)")

    plt.tight_layout()
    plt.savefig(f"{results_dir}/waveform.png")
    plt.close()
    print(f"Plotted results to {results_dir}/waveform.png")


if __name__ == "__main__":
    results_dir = sys.argv[1] if len(sys.argv) > 1 else RESULTS_DIR
    run(results_dir)

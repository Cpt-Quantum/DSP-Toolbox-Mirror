import sys

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy import signal

matplotlib.use("Agg")  # Headless mode

RESULTS_DIR = "results"
SAMPLE_RATE = 2000
# Matches HOP_SAMPLE and HOP_FREQUENCY in main.c
HOP_TIME = 0.5
HOP_FREQUENCY = 15
# Edges closer than this (in periods) to a sample are skipped when comparing
# the square and sawtooth waves, as the C phase is rounded to 2^-64 of a period
EDGE_TOLERANCE = 1e-9


def near_edge(cycles, edges):
    """True for samples within EDGE_TOLERANCE periods of any of the edges,
    given as fractions of a period"""
    fraction = np.mod(cycles, 1)
    distance = [np.abs(((fraction - e) + 0.5) % 1 - 0.5) for e in edges]
    return np.min(distance, axis=0) < EDGE_TOLERANCE


def references(t):
    """scipy/numpy references for each wave in main.c"""
    # The hop is phase continuous, so the cycles carry on from the change
    hop_cycles = np.where(t < HOP_TIME, 7 * t, 7 * HOP_TIME + HOP_FREQUENCY * (t - HOP_TIME))
    return {
        # (reference, samples to skip)
        "Square": (signal.square(2 * np.pi * 7 * t, duty=0.3), near_edge(7 * t, [0, 0.3])),
        "Triangle": (signal.sawtooth(2 * np.pi * 7 * t, width=0.5), None),
        "Sawtooth": (signal.sawtooth(2 * np.pi * (7 * t + 0.25)), near_edge(7 * t + 0.25, [0])),
        "Chirp": (signal.chirp(t, f0=10, t1=1, f1=100, method="linear"), None),
        "Hop": (np.sin(2 * np.pi * hop_cycles), None),
        "Mixer real": (np.cos(2 * np.pi * -5 * t), None),
        "Mixer imag": (np.sin(2 * np.pi * -5 * t), None),
    }


def run(results_dir):
    df = pd.read_csv(f"{results_dir}/nco_data.csv")
    # The exact sample times, rather than the float times in the file
    t = np.arange(len(df)) / SAMPLE_RATE

    # The NCO phase is exact, so the errors are from the float sine table and
    # arithmetic, around 3e-7, and don't grow with time
    refs = references(t)
    for name, (ref, skip) in refs.items():
        keep = np.ones_like(t, dtype=bool) if skip is None else ~skip
        error = np.max(np.abs(df[name].values[keep] - ref[keep]))
        skipped = f" ({np.sum(~keep)} sample(s) on an edge skipped)" if skip is not None else ""
        print(f"{name + ':':11s} max error against scipy {error:.3g}{skipped}")

    fig, axes = plt.subplots(len(refs) - 1, 1, figsize=(10, 2 * (len(refs) - 1)), sharex=True)
    for ax, name in zip(axes, [n for n in refs if not n.startswith("Mixer")]):
        ax.plot(t, refs[name][0], label="scipy", alpha=0.4, linewidth=3)
        ax.plot(t, df[name], label="C")
        ax.set_title(name)
    axes[4].axvline(HOP_TIME, color="grey", linestyle="--", linewidth=1)
    mixer = axes[-1]
    mixer.plot(t, df["Mixer real"], label="real")
    mixer.plot(t, df["Mixer imag"], label="imag")
    mixer.set_title("Complex mixer, e^(-j 2 pi 5 t)")
    mixer.legend(loc="upper right")
    axes[0].legend(loc="upper right")
    axes[-1].set_xlabel("Time (s)")

    plt.tight_layout()
    plt.savefig(f"{results_dir}/nco.png")
    plt.close()
    print(f"Plotted results to {results_dir}/nco.png")


if __name__ == "__main__":
    results_dir = sys.argv[1] if len(sys.argv) > 1 else RESULTS_DIR
    run(results_dir)

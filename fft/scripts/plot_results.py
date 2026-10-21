import sys

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

matplotlib.use("Agg")  # Headless mode

RESULTS_DIR = "results"


def load_complex(csv_file):
    df = pd.read_csv(csv_file)
    return df.iloc[:, 0].values, df["Real"].values + 1j * df["Imag"].values


def relative_error(actual, expected):
    return np.max(np.abs(actual - expected)) / np.max(np.abs(expected))


def run(results_dir):
    t, x = load_complex(f"{results_dir}/input_data.csv")
    f, x_fft = load_complex(f"{results_dir}/fft_data.csv")
    _, x_ifft = load_complex(f"{results_dir}/ifft_data.csv")
    _, x_rfft = load_complex(f"{results_dir}/rfft_data.csv")
    x_irfft = pd.read_csv(f"{results_dir}/irfft_data.csv")["Amplitude"].values

    n = len(x)
    ref_fft = np.fft.fft(x)

    # The C transform runs in single precision, so expect errors around 1e-7
    print(f"FFT relative error against numpy: {relative_error(x_fft, ref_fft):.3g}")
    print(f"IFFT round trip relative error:   {relative_error(x_ifft, x):.3g}")
    print(f"RFFT relative error against numpy: {relative_error(x_rfft, np.fft.rfft(x.real)):.3g}")
    print(f"IRFFT round trip relative error:   {relative_error(x_irfft, x.real):.3g}")

    # Single sided amplitude spectrum, matching the amplitude of each input wave
    half = n // 2
    amplitude = 2 * np.abs(x_fft[:half]) / n
    ref_amplitude = 2 * np.abs(ref_fft[:half]) / n

    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 10))

    ax1.plot(t, x.real, label="Input")
    ax1.plot(t, x_ifft.real, "--", label="IFFT output")
    ax1.plot(t, x_irfft, ":", label="IRFFT output")
    ax1.set_title("Time Domain")
    ax1.set_xlabel("Time (s)")
    ax1.legend()

    ax2.plot(f[:half], ref_amplitude, label="numpy", alpha=0.4, linewidth=3)
    ax2.plot(f[:half], amplitude, label="C FFT")
    ax2.plot(f[:half], 2 * np.abs(x_rfft[:half]) / n, "--", label="C real FFT")
    ax2.set_title("Amplitude Spectrum")
    ax2.set_xlabel("Frequency (Hz)")
    ax2.legend()

    ax3.semilogy(f[:half], np.abs(x_fft[:half] - ref_fft[:half]) + 1e-12)
    ax3.set_title("Absolute Error Against numpy")
    ax3.set_xlabel("Frequency (Hz)")

    plt.tight_layout()
    plt.savefig(f"{results_dir}/fft.png")
    plt.close()
    print(f"Plotted results to {results_dir}/fft.png")


if __name__ == "__main__":
    results_dir = sys.argv[1] if len(sys.argv) > 1 else RESULTS_DIR
    run(results_dir)

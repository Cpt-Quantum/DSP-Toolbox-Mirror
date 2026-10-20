import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import argparse


def load_data(file_path):
    try:
        df = pd.read_csv(file_path)
        t = df.iloc[:, 0].values
        x = df.iloc[:, 1].values
        dt = np.mean(np.diff(t))
        fs = 1.0 / dt
        return t, x, fs
    except Exception as e:
        print(f"Error loading {file_path}: {e}")
        return None, None, None


def calculate_sar(f_orig, m_orig, nyquist_new):
    """
    Calculates the Signal-to-Alias Ratio in dB.
    Signal = Power in original data below new Nyquist
    Alias Potential = Power in original data above new Nyquist
    """
    # Use power (magnitude squared) for energy calculations
    p_orig = m_orig**2

    signal_mask = f_orig <= nyquist_new
    alias_mask = f_orig > nyquist_new

    p_signal = np.sum(p_orig[signal_mask])
    p_alias = np.sum(p_orig[alias_mask])

    if p_alias == 0:
        return float("inf")

    sar = 10 * np.log10(p_signal / p_alias)
    return sar


def compare_spectra(original_file, processed_file):
    t_orig, x_orig, fs_orig = load_data(original_file)
    t_proc, x_proc, fs_proc = load_data(processed_file)

    if t_orig is None or t_proc is None:
        return

    def get_fft(signal, fs):
        n = len(signal)
        freqs = np.fft.rfftfreq(n, d=1 / fs)
        # Normalize magnitude by N
        mag = np.abs(np.fft.rfft(signal)) / n
        return freqs, mag

    f_orig, m_orig = get_fft(x_orig, fs_orig)
    f_proc, m_proc = get_fft(x_proc, fs_proc)

    nyquist_new = fs_proc / 2
    sar_val = calculate_sar(f_orig, m_orig, nyquist_new)

    # Convert to dB for plotting
    m_orig_db = 20 * np.log10(m_orig + 1e-12)
    m_proc_db = 20 * np.log10(m_proc + 1e-12)

    # --- Plotting ---
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))

    # Top Plot: Original
    ax1.plot(f_orig, m_orig_db, label="Original Spectrum", color="steelblue", alpha=0.7)
    ax1.axvline(
        nyquist_new,
        color="red",
        linestyle="--",
        label=f"New Nyquist ({nyquist_new:.1f} Hz)",
    )
    ax1.fill_between(
        f_orig,
        m_orig_db.min(),
        m_orig_db,
        where=(f_orig > nyquist_new),
        color="red",
        alpha=0.2,
        label="Alias Source Region",
    )
    ax1.set_title(f"Original Signal (SAR Potential: {sar_val:.2f} dB)")
    ax1.set_ylabel("Magnitude (dB)")
    ax1.legend()

    # Bottom Plot: Processed
    ax2.plot(f_proc, m_proc_db, label="Processed Spectrum", color="darkorange")
    ax2.set_title(f"Processed Signal (fs_new = {fs_proc:.1f} Hz)")
    ax2.set_xlabel("Frequency (Hz)")
    ax2.set_ylabel("Magnitude (dB)")
    ax2.set_xlim([0, nyquist_new])
    ax2.legend()

    plt.tight_layout()

    print("-" * 30)
    print(f"Signal-to-Alias Ratio (SAR): {sar_val:.2f} dB")
    if sar_val > 60:
        print("Status: EXCELLENT. Aliasing is likely below the noise floor.")
    elif sar_val > 40:
        print("Status: GOOD. Minimal aliasing interference.")
    else:
        print(
            "Status: WARNING. Significant energy exists above Nyquist. Check your anti-aliasing filter."
        )
    print("-" * 30)

    plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("original")
    parser.add_argument("processed")
    args = parser.parse_args()
    compare_spectra(args.original, args.processed)

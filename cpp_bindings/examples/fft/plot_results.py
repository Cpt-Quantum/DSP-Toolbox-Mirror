import numpy as np
import matplotlib.pyplot as plt
import argparse


def main():
    parser = argparse.ArgumentParser(description='Plot the FFT of the input data')
    parser.add_argument('--fs', type=float, required=True)
    parser.add_argument('--input', type=str, required=True)
    parser.add_argument('--output', type=str, required=True)
    args = parser.parse_args()

    # Load the input samples, and the bins as interleaved real and imaginary
    x = np.fromfile(args.input, dtype=np.float32)
    bins = np.fromfile(args.output, dtype=np.float32).view(np.complex64)

    # The same transform in numpy, as the reference
    ref_bins = np.fft.rfft(x.astype(np.float64))

    # Single sided amplitude spectrum, so a tone's peak is its amplitude
    def amplitude_db(b):
        return 20 * np.log10(np.maximum(np.abs(b) * 2.0 / len(x), 1e-12))

    f = np.fft.rfftfreq(len(x), d=1.0 / args.fs)

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

    # Short section of the input signal
    n = min(len(x), 500)
    ax1.plot(np.arange(n) / args.fs, x[:n])
    ax1.set_title(f'Input signal (first {n} of {len(x)} samples)')
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Amplitude')

    # Spectrum from the C++ FFT against numpy
    ax2.plot(f, amplitude_db(bins), label='C++ rfft')
    ax2.plot(f, amplitude_db(ref_bins), '--', label='numpy.fft.rfft')
    ax2.set_title(f'{len(x)} point FFT')
    ax2.set_xlabel('Frequency (Hz)')
    ax2.set_ylabel('Amplitude (dB)')
    ax2.legend()

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()

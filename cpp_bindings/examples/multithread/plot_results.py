import json
import argparse
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description='Plot the FIR and IIR filter outputs')
    parser.add_argument('--channels', type=int, required=True)
    parser.add_argument('--chunk', type=int, required=True)
    parser.add_argument('--fs', type=float, required=True)
    parser.add_argument('--filters', type=str, required=True)
    parser.add_argument('--input', type=str, required=True)
    parser.add_argument('--fir', type=str, required=True)
    parser.add_argument('--iir-sos', type=str, required=True)
    parser.add_argument('--iir-ba', type=str, required=True)
    parser.add_argument('--channel', type=int, default=0, help='Channel to plot')
    args = parser.parse_args()

    # Load the binary files and extract one channel
    # Format: [TimeSteps, Channels, SamplesPerChunk]
    def load_channel(filename):
        raw = np.fromfile(filename, dtype=np.float32)
        return raw.reshape(-1, args.channels, args.chunk)[:, args.channel, :].flatten()

    x = load_channel(args.input)
    y_fir = load_channel(args.fir)
    y_sos = load_channel(args.iir_sos)
    y_ba = load_channel(args.iir_ba)

    # Load the filter designs
    with open(args.filters) as f:
        filters = json.load(f)
    fir_b = np.array(filters['fir']['b'])
    iir_sos = np.array(filters['iir_sos']['sections'])
    iir_ba = (np.array(filters['iir_ba']['b']), np.array(filters['iir_ba']['a']))

    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 9))

    # Designed frequency responses
    w, h_fir = signal.freqz(fir_b, 1, worN=4096, fs=args.fs)
    _, h_sos = signal.sosfreqz(iir_sos, worN=4096, fs=args.fs)
    _, h_ba = signal.freqz(*iir_ba, worN=4096, fs=args.fs)
    ax1.plot(w, 20 * np.log10(np.maximum(np.abs(h_fir), 1e-12)), label=f'FIR ({len(fir_b)} taps)')
    ax1.plot(w, 20 * np.log10(np.maximum(np.abs(h_sos), 1e-12)), label=f'IIR SOS ({len(iir_sos)} sections)')
    ax1.plot(w, 20 * np.log10(np.maximum(np.abs(h_ba), 1e-12)), '--', label=f'IIR direct form (order {len(iir_ba[1]) - 1})')
    ax1.set_ylim(-100, 5)
    ax1.set_title('Designed frequency response')
    ax1.set_xlabel('Frequency (Hz)')
    ax1.set_ylabel('Magnitude (dB)')
    ax1.legend()

    # Spectrum of the channel before and after filtering
    for data, label, style in ((x, 'Input', '-'), (y_fir, 'FIR output', '-'),
                               (y_sos, 'IIR SOS output', '-'),
                               (y_ba, 'IIR direct form output', '--')):
        f, pxx = signal.welch(data, fs=args.fs, nperseg=1024)
        ax2.plot(f, 10 * np.log10(np.maximum(pxx, 1e-20)), style, label=label)
    ax2.set_title(f'Channel {args.channel} power spectral density')
    ax2.set_xlabel('Frequency (Hz)')
    ax2.set_ylabel('PSD (dB/Hz)')
    ax2.legend()

    # Short section of the time domain signals
    n = min(len(x), 1000)
    t = np.arange(n) / args.fs
    ax3.plot(t, x[:n], label='Input', alpha=0.5)
    ax3.plot(t, y_fir[:n], label='FIR output')
    ax3.plot(t, y_sos[:n], label='IIR SOS output')
    ax3.plot(t, y_ba[:n], '--', label='IIR direct form output')
    ax3.set_title(f'Channel {args.channel} time domain')
    ax3.set_xlabel('Time (s)')
    ax3.legend()

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()

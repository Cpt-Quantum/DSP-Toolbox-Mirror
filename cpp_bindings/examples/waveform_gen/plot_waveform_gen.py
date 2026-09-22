import csv
import sys

import matplotlib.pyplot as plt

DEFAULT_CSV = "results/output_file.csv"


def load(csv_file):
    with open(csv_file, newline="") as f:
        reader = csv.reader(f)
        meta = dict(zip(*[iter(next(reader))] * 2))
        channels = next(reader)[1:]

        t = []
        data = [[] for _ in channels]
        for row in reader:
            t.append(float(row[0]))
            for i, value in enumerate(row[1:]):
                data[i].append(float(value))

    return meta, t, channels, data


def plot(csv_file):
    meta, t, channels, data = load(csv_file)

    fig, ax = plt.subplots()
    for ch, series in zip(channels, data):
        ax.plot(t, series, linewidth=0.5, alpha=0.6)

    ax.set_xlabel("Time")
    ax.set_ylabel("Amplitude")
    ax.set_title(
        f"{meta['Channels']} channels, wavelength {meta['Wavelength']}, "
        f"sample rate {meta['Sample rate']}"
    )

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    csv_file = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_CSV
    plot(csv_file)

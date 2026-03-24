import sys
import matplotlib.pyplot as plt
from scipy.io import loadmat


def inspect(mat_file):
    data = loadmat(mat_file)

    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=False)

    ax1.plot(data["t_in"].flatten(), data["sig_in"].flatten(), label="Input", alpha=0.4)
    ax1.plot(data["t_out"].flatten(), data["sig_out"].flatten(), label="Filtered")
    ax1.legend()
    ax1.set_title("Time Domain")

    ax2.plot(
        data["f_in"].flatten(), data["m_in"].flatten(), label="Input FFT", alpha=0.4
    )
    ax2.plot(data["f_out"].flatten(), data["m_out"].flatten(), label="Filtered FFT")
    ax2.legend()
    ax2.set_title("Frequency Domain (dB)")

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    inspect(sys.argv[1])

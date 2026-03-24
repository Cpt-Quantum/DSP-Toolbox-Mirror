import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.io import savemat
import matplotlib

matplotlib.use("Agg")  # Headless mode


def run_pipeline(input_csv, filtered_csv):
    # 1. Load & Process
    df_in = pd.read_csv(input_csv)
    df_out = pd.read_csv(filtered_csv)

    t_in, sig_in = df_in["Time"].values, df_in["Amplitude"].values
    t_out, sig_out = df_out["Time"].values, df_out["Amplitude"].values

    def get_fft(sig):
        n = len(sig)
        f = np.fft.rfftfreq(n)
        m = 20 * np.log10(np.abs(np.fft.rfft(sig)) + 1e-9)
        return f, m

    f_in, m_in = get_fft(sig_in)
    f_out, m_out = get_fft(sig_out)

    # 2. Save Data to .mat (The Bridge)
    mat_name = filtered_csv.replace(".csv", ".mat")
    data_dict = {
        "t_in": t_in,
        "sig_in": sig_in,
        "f_in": f_in,
        "m_in": m_in,
        "t_out": t_out,
        "sig_out": sig_out,
        "f_out": f_out,
        "m_out": m_out,
        "source": filtered_csv,
    }
    savemat(mat_name, data_dict)

    # 3. Save Static PNG
    plt.figure(figsize=(10, 8))
    plt.subplot(2, 1, 1)
    plt.plot(t_in, sig_in, alpha=0.3)
    plt.plot(t_out, sig_out)
    plt.subplot(2, 1, 2)
    plt.plot(f_in, m_in, alpha=0.3)
    plt.plot(f_out, m_out)
    plt.savefig(filtered_csv.replace(".csv", ".png"))
    plt.close()
    print(f"Processed {filtered_csv} -> .mat and .png")


if __name__ == "__main__":
    run_pipeline(sys.argv[1], sys.argv[2])

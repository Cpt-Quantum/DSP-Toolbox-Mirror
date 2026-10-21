import sys

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

matplotlib.use("Agg")  # Headless mode

RESULTS_DIR = "results"
# Must match DETREND_LENGTH and POWER_LENGTH in main.c
DETREND_LENGTH = 4000
POWER_LENGTH = 40


def max_difference(actual, expected):
    return np.max(np.abs(actual - expected))


def run(results_dir):
    df_in = pd.read_csv(f"{results_dir}/input_data.csv")
    df_detrend = pd.read_csv(f"{results_dir}/detrend_data.csv")
    df_power = pd.read_csv(f"{results_dir}/power_data.csv")
    t, x = df_in["Time"].values, df_in["Amplitude"].values

    # numpy references for the block average detrend and the power
    block_avg = x.reshape(-1, DETREND_LENGTH).mean(axis=1)
    ref_block = x - np.repeat(block_avg, DETREND_LENGTH)
    ref_power = (ref_block.reshape(-1, POWER_LENGTH) ** 2).mean(axis=1)

    # The int32 input has 20 fractional bits, so expect differences around 1e-6
    print(f"DC offset of the input:             {np.mean(x):.6g}")
    for method in ["Block", "Moving"]:
        print(f"Remaining DC offset ({method + '):':8s}      "
              f"float {np.mean(df_detrend[method]):.3g}, int32 {np.mean(df_detrend[method + '_int32']):.3g}")
    print(f"Block detrend max error vs numpy:   float {max_difference(df_detrend['Block'], ref_block):.3g}, "
          f"int32 {max_difference(df_detrend['Block_int32'], ref_block):.3g}")
    print(f"Power max relative error vs numpy:  float "
          f"{max_difference(df_power['Power'], ref_power) / np.max(ref_power):.3g}, int32 "
          f"{max_difference(df_power['Power_int32'], ref_power) / np.max(ref_power):.3g}")

    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 10))

    # Show the first 50ms, where the moving average is still filling
    shown = t < 0.05
    ax1.plot(t[shown], x[shown], label="Input", alpha=0.4)
    ax1.plot(t[shown], df_detrend["Block"][shown], label="Block average")
    ax1.plot(t[shown], df_detrend["Moving"][shown], label="Moving average")
    ax1.set_title("DC Offset Removal (first 50ms)")
    ax1.set_xlabel("Time (s)")
    ax1.legend()

    ax2.plot(df_detrend["Time"], df_detrend["Block_int32"] - df_detrend["Block"], label="Block average")
    ax2.plot(df_detrend["Time"], df_detrend["Moving_int32"] - df_detrend["Moving"], label="Moving average")
    ax2.set_title("int32 Minus float")
    ax2.set_xlabel("Time (s)")
    ax2.legend()

    ax3.plot(df_power["Time"], df_power["Power"], label="float")
    ax3.plot(df_power["Time"], df_power["Power_int32"], "--", label="int32")
    ax3.set_title("Power per 1ms")
    ax3.set_xlabel("Time (s)")
    ax3.legend()

    plt.tight_layout()
    plt.savefig(f"{results_dir}/average.png")
    plt.close()
    print(f"Plotted results to {results_dir}/average.png")


if __name__ == "__main__":
    results_dir = sys.argv[1] if len(sys.argv) > 1 else RESULTS_DIR
    run(results_dir)

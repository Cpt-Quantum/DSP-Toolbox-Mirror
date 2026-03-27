import numpy as np
import scipy.signal as signal
import json

# Setup the filter constants
f_s = 16000.0  # Sampling frequency
f_pb = 2000.0  # Passband frequency
f_sb = 4000.0  # Stopband frequency
filt_gain = 40  # Sets the filter amplitude reduction at the stopband (used for IIR)
fir_taps = 100  # Sets the number of coefficients to design for in the FIR design

# Output filename
filename = "results/filters"

# Design the base IIR (a, b)
iir_b, iir_a = signal.iirdesign(
    wp=f_pb,
    ws=f_sb,
    gpass=1,
    gstop=filt_gain,
    fs=f_s,
    output="ba",
)
# Design the SoS IIR
sos = signal.iirdesign(
    wp=f_pb,
    ws=f_sb,
    gpass=1,
    gstop=filt_gain,
    fs=f_s,
    output="sos",
)
# Design the FIR
fir_b = signal.remez(
    fir_taps,
    [0, f_pb, f_sb, f_s / 2],
    [1, 0],
    fs=f_s,
)

# Structure the data into a dictionary
filter_data = {
    "f_s": f_s,
    "fir": {"b": fir_b.tolist()},
    "iir_base": {"b": iir_b.tolist(), "a": iir_a.tolist()},
    "iir_sos": {"sections": sos.tolist(), "gain": 1.0},  # Room for future expansion!
}

# Write to a JSON file
json_filename = filename + ".json"
with open(json_filename, "w") as f:
    json.dump(filter_data, f, indent=4)

print(f"Filter coefficients exported to {json_filename}")
